#include <vector>
#ifdef INFINIT_WINDOWS
# include <winsock2.h>
# include <shlobj.h>
#endif

#include <functional>

#include <QDesktopServices>
#include <QtConcurrentRun>
#include <QTimer>
#include <QUrl>
#include <QDir>
#include <QVector>

#include <elle/log.hh>
#include <elle/os/environ.hh>

#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/regexp.hh>

ELLE_LOG_COMPONENT("infinit.FIST.State");

namespace fist
{
  static State* g_state = nullptr;

  void
  State::GapDeleter::operator() (gap_State* state) const
  {
    gap_free(state);
  }

  State::State()
    : _state()
    , _login_future()
    , _login_watcher()
    , _register_future()
    , _register_watcher()
    , _ghost_code_future()
    , _ghost_code_watcher()
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
    "",
    download_folder));

    connect(&this->_login_watcher, SIGNAL(finished()),
            this, SLOT(_on_login_result_ready()));
    connect(&this->_register_watcher, SIGNAL(finished()),
            this, SLOT(_on_register_result_ready()));
    connect(&this->_ghost_code_watcher, SIGNAL(finished()),
            this, SLOT(_on_ghost_code_result_ready()));

    ELLE_TRACE_SCOPE("%s: construction", *this);
    g_state = this;

    // Callbacks.
    gap_update_user_callback(
      this->state(),
      [this] (surface::gap::User const& user)
      {
        this->State::on_user_updated(user);
      });
    gap_deleted_swagger_callback(
      this->state(),
      [this] (uint32_t id)
      {
        this->on_swagger_deleted(id);
      });
    gap_user_status_callback(
      this->state(),
      [this] (uint32_t id, bool status)
      {
        this->on_user_status_changed(id, status);
      });
    gap_avatar_available_callback(
      this->state(),
      [this] (uint32_t id)
      {
        this->on_avatar_available(id);
      });
    gap_connection_callback(
      this->state(),
      [this] (bool status, bool retrying, std::string const& message)
      {
        this->on_connection_changed(status, retrying, message);
      });
    gap_peer_transaction_callback(
      this->state(),
      [this] (surface::gap::PeerTransaction const& transaction)
      {
        this->on_peer_transaction_updated(transaction);
      });
    gap_link_callback(
      this->state(),
      [this] (surface::gap::LinkTransaction const& transaction)
      {
        this->on_link_updated(transaction);
      });
    connect(&this->_search_watcher, SIGNAL(finished()),
            this, SLOT(_on_search_results_ready()));

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
  State::login(std::string const& email,
               std::string const& password)
  {
    this->_login_future = QtConcurrent::run(
      [=] {
        auto res = gap_login(this->state(), email, password);
        if (res == gap_ok)
          this->_me = gap_self_id(this->state());
        return res;
      });
    this->_login_watcher.setFuture(this->_login_future);
  }

  void
  State::facebook_connect(std::string const& token,
                          std::string const& email_)
  {
    this->_login_future = QtConcurrent::run(
      [=] {
        auto email = elle::os::getenv("FACEBOOK_PREFERRED_EMAIL", email_);
        auto optional_email = [&] {
          if (email.empty())
            return boost::optional<std::string>{};
          return boost::optional<std::string>{email};
        };
        auto res = gap_facebook_connect(this->state(), token, optional_email());
        if (res == gap_ok)
          this->_me = gap_self_id(this->state());
        return res;
      });
    this->_login_watcher.setFuture(this->_login_future);
  }

  void
  State::_on_login_result_ready()
  {
    emit login_result(this->_login_future.result());
  }

  void
  State::register_(std::string const& fullname,
                   std::string const& email,
                   std::string const& password)
  {
    this->_register_future = QtConcurrent::run(
      [=] {
        // Will explode if the state is destroyed.
        auto res = gap_register(this->state(), fullname, email, password);
        if (res == gap_ok)
          this->_me = gap_self_id(this->state());
        return res;
      });
    this->_register_watcher.setFuture(this->_register_future);
  }

  void
  State::_on_register_result_ready()
  {
    emit register_result(this->_register_future.result());
  }

  void
  State::on_connection_changed(
    bool status,
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
  State::use_ghost_code(std::string const& code)
  {
    this->_ghost_code_future = QtConcurrent::run(
      [=] {
        return gap_use_ghost_code(this->state(), code);
      });
    this->_ghost_code_watcher.setFuture(this->_ghost_code_future);
  }

  void
  State::_on_ghost_code_result_ready()
  {
    emit ghost_code_result(this->_ghost_code_future.result());
  }

  void
  State::on_logged_in()
  {
    ELLE_TRACE("load swaggers")
    {
      std::vector<surface::gap::User> users;
      gap_Status res = gap_swaggers(this->state(), users);
      if (res != gap_ok)
        ELLE_ERR("getting swaggers failed %s", res);
      for (auto const& user: users)
      {
        this->_users[user.id].reset(new model::User(*this, user));
        ELLE_DEBUG("user: %s", *this->_users[user.id]);
      }
    }

    ELLE_TRACE("load transactions")
    {
      std::vector<surface::gap::PeerTransaction> transactions;
      gap_Status res = gap_peer_transactions(this->state(), transactions);
      if (res != gap_ok)
        ELLE_ERR("getting transactions failed %s", res);
      for (auto const& transaction: transactions)
      {
        this->_transactions.emplace(*this, transaction);
        ELLE_TRACE("transaction: %s",
                   *this->_transactions.get<0>().find(transaction.id));
      }
      this->_compute_active_transactions();
    }

    ELLE_TRACE("load links")
    {
      std::vector<surface::gap::LinkTransaction> links;
      gap_Status res = gap_link_transactions(this->state(), links);
      if (res != gap_ok)
        ELLE_ERR("getting transactions failed %s", res);
      for (auto const& link: links)
      {
        if (link.status != gap_transaction_canceled &&
            link.status != gap_transaction_failed &&
            link.status != gap_transaction_deleted)
          this->_links.emplace(*this, link);
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

  model::User const&
  State::me()
  {
    return this->user(this->_me);
  }

  void
  State::on_avatar_available(uint32_t id)
  {
    ELLE_TRACE_SCOPE("%s: avatar available for id %s", *this, id);
    if (this->_users.find(id) == this->_users.end())
      this->_users[id].reset(new model::User(*this, id));
    ELLE_DEBUG("update %s avatar", *this->_users[id])
      this->_users[id]->avatar_updated();
  }

  void
  State::on_swagger_deleted(uint32_t id)
  {
    if (this->_users.find(id) == this->_users.end())
      this->_users[id].reset(new model::User(*this, id));
    this->_users.at(id)->deleted(true);
  }

  void
  State::on_user_updated(surface::gap::User const& user)
  {
    ELLE_TRACE_SCOPE("%s: peer %s updated", *this, user);
    if (this->_users.find(user.id) == this->_users.end())
      this->_users[user.id].reset(new model::User(*this, user));
  }

  void
  State::on_user_status_changed(uint32_t id, bool status)
  {
    if (this->_users.find(id) == this->_users.end())
      this->_users[id].reset(new model::User(*this, id));
    this->_users[id]->status(status);
  }

  State::Users
  State::swaggers(std::function<bool (model::User const&)> filter)
  {
    State::Users res;
    for (auto const& user: this->_users)
    {
      if (user.second->swagger() && filter(*(user.second)))
        res.push_back(user.first);
    }
    return res;
  }

  // Return a subset of the swaggers.
  State::Users
  State::swaggers(QString const& filter)
  {
    return this->swaggers(
      [&filter] (model::User const& user) -> bool
      {
        return (user.fullname().toLower().contains(filter.toLower()) ||
                user.handle().toLower().contains(filter.toLower()));
      });
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
            std::vector<uint32_t> users;
            if (filter.count('@') == 1 && regexp::email::checker.exactMatch(filter))
            {
              surface::gap::User u;
              auto res = gap_user_by_email(this->state(), text.c_str(), u);
              if (res == gap_ok)
                users.push_back(u.id);
              else
                ELLE_WARN("user by email failed: %s", res);
            }
            else
            {
              std::vector<surface::gap::User> _users;
              auto res = gap_users_search(this->state(), text.c_str(), _users);
              if (res == gap_ok)
                for (auto const& user: _users)
                  users.push_back(user.id);
              else
                ELLE_WARN("user search failed: %s", res);
            }
            return users;
          });
      this->_search_watcher.setFuture(this->_search_future);
    }
    return this->swaggers(filter);
  }

  uint32_t
  State::user_id(std::string const& email)
  {
    surface::gap::User u;
    auto res = gap_user_by_email(this->state(), email.c_str(), u);
    if (res == gap_ok)
    {
      if (this->_users.find(u.id) == this->_users.end())
        this->_users[u.id].reset(new model::User(*this, u));
      return u.id;
    }
    else
      ELLE_WARN("user by email failed: %s", res);
    return gap_null();

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
    ELLE_ASSERT(user_id != gap_null());
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
  State::_on_search_results_ready()
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
    emit search_results_ready();
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
  State::on_peer_transaction_updated(surface::gap::PeerTransaction const& tr)
  {
    ELLE_TRACE_SCOPE("%s: transaction notification %s", *this, tr);
    auto id = tr.id;
    ELLE_ASSERT(id != gap_null());
    auto it = this->_transactions.get<0>().find(id);
    if (it == this->_transactions.get<0>().end())
    {
      this->_transactions.emplace(*this, tr);
      emit new_transaction(id);
      it = this->_transactions.get<0>().find(id);
    }
    struct UpdateStatus
    {
      UpdateStatus(surface::gap::PeerTransaction const& transaction):
        _transaction(transaction)
      {}

      void
      operator()(model::Transaction& model)
      {
        model.transaction(this->_transaction);
      }

      ELLE_ATTRIBUTE(surface::gap::PeerTransaction, transaction)
    };
    this->_transactions.modify(it, UpdateStatus(tr));
  }

  void
  State::on_link_updated(surface::gap::LinkTransaction const& link)
  {
    ELLE_TRACE_SCOPE("%s: link notification %s", *this, link);
    auto id = link.id;
    ELLE_ASSERT(id != gap_null());
    auto it = this->_links.get<0>().find(id);
    if (it == this->_links.get<0>().end())
    {
      auto status = link.status;
      if (status == gap_transaction_canceled ||
          status == gap_transaction_failed ||
          status == gap_transaction_deleted)
        return;
      ELLE_TRACE("insert new link to the map");
      this->_links.emplace(*this, link);
      emit new_link(id);
      it = this->_links.get<0>().find(id);
    }

    struct UpdateLink
    {
      UpdateLink(surface::gap::LinkTransaction const& link)
        : _link(link)
      {}

      void
      operator()(model::Link& model)
      {
        model.link(this->_link);
      }

      ELLE_ATTRIBUTE(surface::gap::LinkTransaction, link);
    };
    this->_links.modify(it, UpdateLink(link));
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

  surface::gap::PeerTransaction
  State::_force_transaction_status(uint32_t id, gap_TransactionStatus status) const
  {
    auto it = this->_transactions.get<0>().find(id);
    ELLE_ASSERT(it != this->_transactions.get<0>().end());
    surface::gap::PeerTransaction tr = it->_transaction;
    tr.status = status;
    return tr;
  }

  surface::gap::LinkTransaction
  State::_force_link_status(uint32_t id, gap_TransactionStatus status) const
  {
    auto it = this->_links.get<0>().find(id);
    ELLE_ASSERT(it != this->_links.get<0>().end());
    surface::gap::LinkTransaction link = it->_link;
    link.status = status;
    return link;
  }

  void
  State::on_transaction_accepted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    QDir download_folder = QDir::fromNativeSeparators(this->download_folder());
    if (!download_folder.exists())
      emit new_download_folder_needed();
    else
    {
      this->on_peer_transaction_updated(
        this->_force_transaction_status(id, gap_transaction_connecting));
      gap_accept_transaction(this->state(), id);
    }
  }

  void
  State::on_transaction_rejected(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    this->on_peer_transaction_updated(
      this->_force_transaction_status(id, gap_transaction_rejected));
    gap_reject_transaction(this->state(), id);
  }

  void
  State::on_transaction_canceled(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    this->on_peer_transaction_updated(
      this->_force_transaction_status(id, gap_transaction_canceled));
    gap_cancel_transaction(this->state(), id);
  }

  void
  State::on_transaction_deleted(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    this->on_link_updated(
      this->_force_link_status(id, gap_transaction_deleted));;
    gap_delete_transaction(this->state(), id);
  }

  void
  State::open_file(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->download_folder()));
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
