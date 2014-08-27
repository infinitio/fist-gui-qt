# include <vector>

# include <QDesktopServices>
# include <QtConcurrentRun>
# include <QTimer>
# include <QUrl>
# include <QVector>

# include <elle/log.hh>

# include <fist-gui-qt/model/User.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.State");

namespace fist
{

  namespace
  {
    static const QRegExp email_checker(regexp::email,
                                       Qt::CaseInsensitive);
  }

  static State* g_state = nullptr;

  void
  State::GapDeleter::operator() (gap_State* state) const
  {
    gap_free(state);
  }

  State::State(gap_State* state)
    : _state(state)
    , _users()
    , _search_future()
    , _search_watcher()
    , _last_results()
    , _transactions()
    , _active_transactions()
    , _links()
    , _active_links()
    , _poll_timer(new QTimer)
  {
    ELLE_TRACE_SCOPE("%s: construction", *this);
    g_state = this;
    ELLE_DEBUG("transaction updated callback")
      gap_transaction_callback(this->state(), State::transaction_callback);
    ELLE_DEBUG("link updated callback")
      gap_link_callback(
        this->state(),
        std::bind(&State::on_link_updated_callback, this, std::placeholders::_1));
    ELLE_DEBUG("user status updated callback")
      gap_user_status_callback(this->state(), State::user_status_callback);
    ELLE_DEBUG("avatar updated callback")
      gap_avatar_available_callback(this->state(), State::avatar_available_callback);

    connect(&this->_search_watcher, SIGNAL(finished()),
            this, SLOT(_on_results_ready()));
  }

  State::~State()
  {
    ELLE_DEBUG_SCOPE("%s: destruction", *this);
    ELLE_DEBUG("destroy poll timer")
      this->_poll_timer.reset();
    ELLE_DEBUG("cancel search")
      this->cancel_search();
  }

  void
  State::on_logged_in()
  {
    ELLE_TRACE("load swaggers")
    {
      uint32_t* swaggers = gap_swaggers(this->state());
      for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
      {
        this->user(swaggers[i]);
        ELLE_DEBUG("user: %s", *this->_users[swaggers[i]]);
      }
      gap_swaggers_free(swaggers);
    }

    ELLE_TRACE("load transactions")
    {
      uint32_t* trs = gap_transactions(this->state());
      for (uint32_t i = 0; trs[i] != gap_null(); ++i)
      {
        this->_transactions.emplace(*this, trs[i]);
        ELLE_DEBUG("transaction: %s", *this->_transactions.get<0>().find(trs[i]));
      }
      gap_transactions_free(trs);
      this->_compute_active_transactions();
    }

    ELLE_TRACE("load links")
    {
      auto const& links = gap_link_transactions(this->state());
      for (auto const& link: links)
      {
        this->_links.emplace(*this, link.id);
      }
      this->_compute_active_links();
    }

    ELLE_DEBUG("start the update loop")
    {
      connect(this->_poll_timer.get(), SIGNAL(timeout()), this, SLOT(_poll()));
      this->_poll_timer->start(1000);
    }
  }

  void
  State::_poll()
  {
    ELLE_DUMP("%s: poll", *this);
    auto res = gap_poll(this->state());

    if (!res)
      ELLE_ERR("poll failed: %s", res);
  }

  void
  State::critical_callback(char const* str)
  {
    g_state->on_critical_callback(str);
  }

  void
  State::on_critical_callback(char const* str)
  {
    ELLE_WARN_SCOPE("%s: critical problem %s", *this, str);
    emit critical_failure(QString(str));
  }

  void
  State::avatar_available_callback(uint32_t id)
  {
    g_state->on_avatar_available_callback(id);
  }

  void
  State::on_avatar_available_callback(uint32_t id)
  {
    ELLE_TRACE_SCOPE("%s: avatar available for id %s", *this, id);
    if (this->_users.find(id) == this->_users.end())
      this->_users[id].reset(new model::User(*this, id));
    ELLE_DEBUG("update %s avatar", *this->_users[id])
      this->_users[id]->avatar_updated();
    for (model::Transaction const& model: this->_transactions.get<0>())
    {
      if (model.peer_id() == id)
        model.avatar_updated();
    }
  }

  void
  State::user_status_callback(uint32_t id, gap_UserStatus status)
  {
    g_state->on_user_status_callback(id, status);
  }

  void
  State::on_user_status_callback(uint32_t id, gap_UserStatus status)
  {
    ELLE_TRACE_SCOPE("%s: peer %s status updated to %s", *this, id, status);
    this->user(id).avatar_updated();
    for (model::Transaction const& model: this->_transactions.get<0>())
    {
      if (model.peer_id() == id)
        model.peer_status_updated();
    }
  }

  State::Users
  State::swaggers()
  {
    uint32_t* swaggers = gap_swaggers(this->state());

    State::Users res;
    for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
      res.push_back(this->user(swaggers[i]).id());
    gap_swaggers_free(swaggers);

    return res;
  }

  // Return a subset of the swaggers.
  State::Users
  State::swaggers(QString const& filter)
  {
    uint32_t* swaggers = gap_swaggers(this->state());

    State::Users res;
    for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
    {
      auto const& user = this->user(swaggers[i]);
      if (user.fullname().toLower().contains(filter.toLower()) ||
          user.handle().toLower().contains(filter.toLower()))
        res.push_back(user.id());
    }
    gap_swaggers_free(swaggers);

    return res;
  }

  State::Users
  State::search(QString const& filter)
  {
    ELLE_DEBUG("search %s", filter);
    ELLE_DEBUG("cancel future")
      this->cancel_search();
    ELLE_DEBUG("make concurent run")
      this->_search_future = QtConcurrent::run(
        [&,filter] {
          std::string text = filter.toStdString();
          if (filter.count('@') == 1 && email_checker.exactMatch(filter))
          {
            return std::vector<uint32_t>{gap_user_by_email(this->state(), text.c_str())};
          }
        else
          return gap_users_search(this->state(), text.c_str());
      });
    this->_search_watcher.setFuture(this->_search_future);
    return this->swaggers(filter);
  }

  // Cancel the search operation.
  void
  State::cancel_search()
  {
    ELLE_DEBUG("cancel search")
      this->_search_future.cancel();
    ELLE_DEBUG("reset future")
      this->_search_future = FutureSearchResult();
    ELLE_DEBUG("restore watcher")
      this->_search_watcher.setFuture(this->_search_future);
  }

  model::User const&
  State::user(uint32_t user_id)
  {
    if (this->_users.find(user_id) == this->_users.end())
      this->_users[user_id].reset(new model::User(*this, user_id));
    return *this->_users[user_id];
  }

  State::Users
  State::results()
  {
    State::Users res;
    for (uint32_t id: this->_last_results)
    {
      if (id != gap_null())
      {
        res.push_back(this->user(id).id());
      }
    }
    return res;
  }

  void
  State::_on_results_ready()
  {
    ELLE_TRACE_SCOPE("%s: results ready", *this);
    this->_last_results.clear();
    auto const& future = this->_search_watcher.future();
    if (future.constBegin() != future.constEnd())
    {
      this->_last_results = this->_search_watcher.result();
    }
    else
    {
      ELLE_DEBUG("future empty");
    }
    emit results_ready();
  }

  void
  State::transaction_callback(uint32_t id, gap_TransactionStatus status)
  {
    ELLE_ASSERT(id != gap_null());
    ELLE_TRACE_SCOPE("transaction %s updated with status %s", id, status);
    g_state->on_transaction_callback(id, status);
  }

  void
  State::on_transaction_callback(uint32_t id, gap_TransactionStatus status)
  {
    ELLE_TRACE_SCOPE("%s: transaction notification (%s) with status %s",
                     *this, id, status);
    ELLE_ASSERT(id != gap_null());
    if (!gap_is_link_transaction(this->state(), id))
    {
      auto it = this->_transactions.get<0>().find(id);
      if (it == this->_transactions.get<0>().end())
      {
        this->_transactions.emplace(*this, id);
        emit new_transaction(id);
      }
      else if (it->status() == status)
        return;

      struct UpdateStatus
      {
        UpdateStatus(gap_TransactionStatus status):
          _status(status)
        {}

        void
        operator()(model::Transaction& model)
        {
          model.status(this->_status);
        }

        ELLE_ATTRIBUTE(gap_TransactionStatus, status);
      };

      this->_transactions.modify(it, UpdateStatus(status));
      this->_transactions.get<0>().find(id)->status_updated();
      emit transaction_updated(id);
      this->_compute_active_transactions();
    }
    else
    {
      auto it = this->_links.get<0>().find(id);
      if (it == this->_links.get<0>().end())
      {
        this->_links.emplace(*this, id);
        emit new_link(id);
      }
      else if (it->status() == status)
        return;

      struct UpdateLink
      {
        UpdateLink() = default;

        void
        operator()(model::Link& model)
        {
          model.update();
        }
      };

      this->_links.modify(it, UpdateLink());
      ELLE_DEBUG("update link")
        emit link_updated(id);
      this->_compute_active_links();
    }
  }

  void
  State::on_link_updated_callback(surface::gap::LinkTransaction const& tr)
  {
    this->on_transaction_callback(tr.id, tr.status);
  }

  model::Transaction const&
  State::transaction(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    auto it = this->_transactions.get<0>().find(id);
    if (it == this->_transactions.get<0>().end())
    {
      this->_transactions.emplace(*this, id);
      emit new_transaction(id);
    }

    return *this->_transactions.get<0>().find(id);
  }

  // XXX: Use a per transaction boolean which says if it's activ or not.
  void
  State::_compute_active_transactions()
  {
    unsigned int count = 0;
    for (auto const& transaction: this->_transactions.get<0>())
    {
      if (!transaction.is_final() &&
          gap_transaction_concern_device(this->state(), transaction.id()))
        ++count;
    }
    this->active_transactions(count);
  }

  void
  State::active_transactions(unsigned int count)
  {
    if (this->_active_transactions != count)
    {
      this->_active_transactions = count;
      emit active_transactions_changed(this->_active_transactions);
    }
  }

  void
  State::on_transaction_accepted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_accept_transaction(this->state(), id);
  }

  void
  State::on_transaction_rejected(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_reject_transaction(this->state(), id);
  }

  void
  State::on_transaction_canceled(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_cancel_transaction(this->state(), id);
  }

  void
  State::on_transaction_deleted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_delete_transaction(this->state(), id);
  }

  void
  State::open_file(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    QDesktopServices::openUrl(QUrl::fromLocalFile((QString(gap_get_output_dir(this->state())))));
  }

  void
  State::_compute_active_links()
  {
    unsigned int count = 0;
    for (auto const& link: this->_links.get<0>())
    {
      if (!link.is_finished())
        ++count;
    }
    this->active_links(count);
  }

  void
  State::active_links(unsigned int count)
  {
    if (this->_active_links != count)
    {
      this->_active_links = count;
      emit active_links_changed(this->_active_links);
    }
  }

  model::Link const&
  State::link(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    auto it = this->_links.get<0>().find(id);
    if (it == this->_links.get<0>().end())
    {
      this->_links.emplace(*this, id);
      emit new_link(id);
    }

    return *this->_links.get<0>().find(id);
  }
}
