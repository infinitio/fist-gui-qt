#include <vector>
#ifdef INFINIT_WINDOWS
# include <winsock2.h>
# include <shlobj.h>
#endif
#include <QDesktopServices>
#include <QtConcurrentRun>
#include <QTimer>
#include <QUrl>
#include <QDir>
#include <QVector>

#include <elle/log.hh>

#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/Settings.hh>

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

  State::State()
    : _state()
    , _users()
    , _search_future()
    , _search_watcher()
    , _last_results()
    , _transactions()
    , _acceptable_transactions(0)
    , _running_transactions(0)
    , _links()
    , _active_links()
    , _poll_timer(new QTimer)
  {
    QString _download_folder = fist::settings()["State"].get("download_folder", "").toString();
    auto array = _download_folder.toUtf8();
    std::string download_folder(array.constData());
  this->_state.reset(gap_new(
#ifdef FIST_PRODUCTION_BUILD
    true,
#else
    false,
#endif
      download_folder));

    ELLE_TRACE_SCOPE("%s: construction", *this);
    g_state = this;
    // Merge Transaction callback & recipient changed callback when new
    // notification system is up.
    ELLE_DEBUG("connect transaction updated callback")
      gap_transaction_callback(this->state(), State::transaction_callback);
    ELLE_DEBUG("connect transaction recipient changed callback");
      gap_transaction_recipient_changed_callback(
        this->state(), State::transaction_recipient_changed_callback);
    ELLE_DEBUG("connect link updated callback")
      gap_link_callback(
        this->state(),
        std::bind(
          &State::on_link_updated_callback, this, std::placeholders::_1));
    ELLE_DEBUG("connect user status updated callback")
      gap_user_status_callback(this->state(), State::user_status_callback);
    ELLE_DEBUG("connect avatar updated callback")
      gap_avatar_available_callback(
        this->state(), State::avatar_available_callback);
    ELLE_DEBUG("connect connection callback")
      gap_connection_callback(this->state(), State::connection_callback);
    ELLE_DEBUG("connect swagger deleted callback")
      gap_deleted_swagger_callback(
        this->state(), State::swagger_deleted_callback);

    connect(&this->_search_watcher, SIGNAL(finished()),
            this, SLOT(_on_results_ready()));

    ELLE_DEBUG("start the update loop")
    {
      connect(this->_poll_timer.get(), SIGNAL(timeout()), this, SLOT(_poll()));
      this->_poll_timer->start(1000);
    }
  }

  State::~State()
  {
    ELLE_DEBUG_SCOPE("%s: destruction", *this);
    g_state = nullptr;
    ELLE_DEBUG("destroy poll timer")
      this->_poll_timer.reset();
    ELLE_DEBUG("cancel search")
      this->cancel_search();
  }

  void
  State::connection_callback(bool status,
                             bool still_retrying,
                             std::string const& reason)
  {
    g_state->on_connection_callback(status, still_retrying, reason);
  }

  void
  State::on_connection_callback(bool status,
                                bool still_retrying,
                                std::string last_error)
  {
    ELLE_TRACE_SCOPE("connection callback: %sconnected will%s retry",
                     status ? "" : "dis",
                     still_retrying ? "" : "n't");
    if (status)
      emit connection_enable();
    else if (status == false && still_retrying)
      emit internet_issue("trying to connect");
    else
      emit kicked_out(QString::fromStdString(last_error));
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
        ELLE_DEBUG("transaction: %s",
                   *this->_transactions.get<0>().find(trs[i]));
      }
      gap_transactions_free(trs);
      this->_compute_active_transactions();
    }

    ELLE_TRACE("load links")
    {
      auto const& links = gap_link_transactions(this->state());
      for (auto const& link: links)
      {
        if (link.status != gap_transaction_canceled &&
            link.status != gap_transaction_failed &&
            link.status != gap_transaction_deleted)
          this->_links.emplace(*this, link.id);
        else
          ELLE_DEBUG("ignore %s", link);
      }
      this->_compute_active_links();
    }
  }

  void
  State::send_metric(UIMetricsType metric,
                     std::unordered_map<std::string, std::string> const& add)
  {
    gap_send_metric(this->state(), metric, add);
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
      if (model.peer_id() == id)
        model.avatar_updated();
  }

  void
  State::swagger_deleted_callback(uint32_t id)
  {
    g_state->on_swagger_deleted(id);
  }

  void
  State::on_swagger_deleted(uint32_t id)
  {
    if (this->_users.find(id) == this->_users.end())
      this->_users[id].reset(new model::User(*this, id));
    this->_users.at(id)->deleted(true);
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
      if (model.peer_id() == id)
        model.peer_status_updated();
  }

  State::Users
  State::swaggers()
  {
    uint32_t* swaggers = gap_swaggers(this->state());

    State::Users res;
    for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
    {
      auto& u = this->user(swaggers[i]);
      if (!u.deleted())
        res.push_back(this->user(swaggers[i]).id());
    }
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
      if (!user.deleted())
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
    if (!filter.isEmpty())
    {
      ELLE_DEBUG("make concurent run")
        this->_search_future = QtConcurrent::run(
          [&,filter] {
            std::string text = filter.toStdString();
            if (filter.count('@') == 1 && email_checker.exactMatch(filter))
              return std::vector<uint32_t>{
                gap_user_by_email(this->state(), text.c_str())};
            else
              return gap_users_search(this->state(), text.c_str());
          });
      this->_search_watcher.setFuture(this->_search_future);
    }
    return this->swaggers(filter);
  }

  uint32_t
  State::user_id(std::string const& email)
  {
    return gap_user_by_email(this->state(), email.c_str());
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

  QString
  State::download_folder() const
  {
    return QString::fromUtf8(gap_get_output_dir(this->state()).c_str());
  }

  void
  State::download_folder(QString const& folder)
  {
    auto array = folder.toUtf8();
    std::string _folder(array.constData());
    gap_set_output_dir(this->state(), _folder, true);
    fist::settings()["State"].set("download_folder", folder);
  }

  void
  State::transaction_callback(uint32_t id, gap_TransactionStatus status)
  {
    ELLE_ASSERT(id != gap_null());
    ELLE_TRACE_SCOPE("transaction %s updated with status %s", id, status);
    g_state->on_transaction_callback(id, status, false);
  }

  void
  State::transaction_recipient_changed_callback(
    uint32_t transaction_id, uint32_t recipient_id)
  {
    g_state->on_transaction_recipient_changed(transaction_id, recipient_id);
  }

  void
  State::on_transaction_recipient_changed(uint32_t transaction_id,
                                          uint32_t recipient_id)
  {
    ELLE_TRACE_SCOPE("%s: peer changed for %s changed to %s",
                     *this, transaction_id, recipient_id);
    ELLE_ASSERT(transaction_id != gap_null());
    ELLE_ASSERT(recipient_id != gap_null());
    ELLE_ASSERT(gap_is_p2p_transaction(this->state(), transaction_id));
    auto it = this->_transactions.get<0>().find(transaction_id);
    if (it != this->_transactions.get<0>().end())
    {
      struct UpdateTransaction
      {
        UpdateTransaction()
        {}

        void
        operator()(model::Transaction& model)
        {
          model.on_peer_changed();
        }
      };
      this->_transactions.modify(it, UpdateTransaction());
    }
  }

  void
  State::on_transaction_callback(uint32_t id,
                                 gap_TransactionStatus status,
                                 bool manual)
  {
    ELLE_TRACE_SCOPE("%s: transaction notification (%s) with status %s",
                     *this, id, status);
    ELLE_ASSERT(id != gap_null());
    if (!gap_is_link_transaction(this->state(), id))
    {
      auto it = this->_transactions.get<0>().find(id);
      if (it == this->_transactions.get<0>().end())
      {
        if (manual)
          return;

        this->_transactions.emplace(*this, id);
        emit new_transaction(id);
        it = this->_transactions.get<0>().find(id);
      }
      else if (it->status() == status)
      {
        return;
      }

      struct UpdateStatus
      {
        UpdateStatus(gap_TransactionStatus status):
          _status(status)
        {}

        void
        operator()(model::Transaction& model)
        {
          model.status(this->_status);
          model.update();
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
        if (manual)
          return;

        if (status == gap_transaction_canceled ||
            status == gap_transaction_failed ||
            status == gap_transaction_deleted)
          return;
        ELLE_TRACE("insert new link to the map");
        this->_links.emplace(*this, id);
        emit new_link(id);
        it = this->_links.get<0>().find(id);
      }

      bool update = false;
      struct UpdateLink
      {
        UpdateLink(gap_TransactionStatus status,
                   bool& update)
          : status(status)
          , update(update)
        {}

        void
        operator()(model::Link& model)
        {
          if (this->status > model._link.status)
          {
            update = true;
            model._link.status = this->status;
          }
          model.update();
        }

        gap_TransactionStatus status;
        bool& update;
      };

      this->_links.modify(it, UpdateLink(status, update));
      ELLE_DEBUG("update link")
        emit link_updated(id);
      if (update)
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

  // XXX: Use a per transaction boolean which says if it's active or not.

  void
  State::_compute_active_transactions()
  {
    size_t acceptable = 0, running = 0;
    for (auto const& transaction: this->_transactions.get<0>())
    {
      if (transaction.acceptable())
        ++acceptable;
      if (transaction.running())
        ++running;
    }
    this->acceptable_transactions(acceptable);
    this->running_transactions(running);
  }

  void
  State::acceptable_transactions(size_t acceptable)
  {
    if (this->_acceptable_transactions != acceptable)
    {
      this->_acceptable_transactions = acceptable;
      emit acceptable_transactions_changed(this->_acceptable_transactions);
    }
  }

  void
  State::running_transactions(size_t running)
  {
    if (this->_running_transactions != running)
    {
      this->_running_transactions = running;
      emit running_transactions_changed(this->_running_transactions);
    }
  }

  void
  State::on_transaction_accepted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    QDir download_folder = QDir::fromNativeSeparators(this->download_folder());
    if (!download_folder.exists())
      emit new_download_folder_needed();
    gap_accept_transaction(this->state(), id);
    this->on_transaction_callback(id, gap_transaction_connecting, true);
  }

  void
  State::on_transaction_rejected(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_reject_transaction(this->state(), id);
    this->on_transaction_callback(id, gap_transaction_rejected, true);
  }

  void
  State::on_transaction_canceled(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_cancel_transaction(this->state(), id);
    this->on_transaction_callback(id, gap_transaction_canceled, true);
  }

  void
  State::on_transaction_deleted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_delete_transaction(this->state(), id);
    this->on_transaction_callback(id, gap_transaction_deleted, true);
  }

  void
  State::open_file(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    QDesktopServices::openUrl(
      QUrl::fromLocalFile(this->download_folder()));
  }

  void
  State::_compute_active_links()
  {
    size_t count = 0;
    for (auto const& link: this->_links.get<0>())
    {
      if (!link.is_finished())
        ++count;
    }
    this->active_links(count);
  }

  void
  State::active_links(size_t count)
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

  QString
  State::facebook_app_id() const
  {
    auto id = QString::fromStdString(gap_facebook_app_id());
    return id;
  }
}
