#include <vector>
#ifdef INFINIT_WINDOWS
# include <winsock2.h>
# include <shlobj.h>
#endif

#include <functional>
#include <algorithm>

#include <QDesktopServices>
#include <QApplication>
#include <QHostInfo>
#include <QBuffer>
#include <QtConcurrentRun>
#include <QTimer>
#include <QUrl>
#include <QDir>
#include <QVector>
#include <boost/range/adaptor/reversed.hpp>

#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/system/platform.hh>
#include <elle/UUID.hh>

#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/notification/Center.hh>
#include <fist-gui-qt/notification/IncomingTransaction.hh>
#include <fist-gui-qt/State.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/regexp.hh>
#include <fist-gui-qt/utils.hh>

#include <fist-gui-qt/CustomEvents/ContactJoined.hh>
#include <fist-gui-qt/CustomEvents/AccountUpdated.hh>
#include <fist-gui-qt/CustomEvents/Events.hh>

# include <surface/gap/gap.hh>

ELLE_LOG_COMPONENT("infinit.FIST.State");


namespace fist
{
  void
  State::GapDeleter::operator() (gap_State* state) const
  {
    gap_free(state);
  }

  State::State()
    : _state()
    , _logged_in(false)
    , _login_future()
    , _login_watcher()
    , _register_future()
    , _register_watcher()
    , _ghost_code_future()
    , _ghost_code_watcher()
    , _my_id(gap_null())
    , _device(elle::UUID().repr())
    , _web_token()
    , _users()
    , _search_future()
    , _search_watcher()
    , _last_results()
    , _transactions()
    , _acceptable_transactions(0)
    , _running_transactions(0)
    , _links()
    , _active_links()
    , _poll_timer(new QTimer(this))
    , fullscreen_screenshot(new QxtGlobalShortcut(this))
    , region_screenshot(new QxtGlobalShortcut(this))
  {
    QString _download_folder = fist::settings()["State"].get("download_folder", "").toString();
    auto download_folder = QString_to_utf8_string(_download_folder);
    this->_state.reset(gap_new(
#ifdef FIST_PRODUCTION_BUILD
                         true,
#else
                         false,
#endif
                         download_folder));
    ELLE_ASSERT(this->_state != nullptr);

    connect(&this->_login_watcher, SIGNAL(finished()),
            this, SLOT(_on_login_result_ready()));
    connect(&this->_register_watcher, SIGNAL(finished()),
            this, SLOT(_on_register_result_ready()));
    connect(&this->_ghost_code_watcher, SIGNAL(finished()),
            this, SLOT(_on_ghost_code_result_ready()));

    ELLE_TRACE_SCOPE("%s: construction", *this);

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
        this->_on_avatar_available(id);
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
    gap_account_changed_callback(
      this->state(),
      [this] (::Account const& account)
      {
        QApplication::postEvent(this, new AccountUpdated(account));
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
    ELLE_LOG_SCOPE("%s: destruction", *this);
    ELLE_DEBUG("destroy poll timer")
    {
      if (this->_poll_timer)
      {
        this->_poll_timer->stop();
        this->_poll_timer.reset();
      }
    }
    ELLE_DEBUG("cancel search")
      this->cancel_search();
  }

  uint32_t
  State::my_id() const
  {
    if (this->_my_id == gap_null())
      this->_my_id = gap_self_id(this->state());
    return this->_my_id;
  }

  std::string
  State::device_id() const
  {
    static const elle::UUID nil_uuid;
    if (this->_device == nil_uuid.repr())
      this->_device = gap_self_device_id(this->state());
    return this->_device;
  }

  model::Device
  State::device() const
  {
    for (auto const& device: this->devices())
      if (device.id() == QString_from_utf8_string(this->device_id()))
        return device;
    elle::unreachable();
  }

  model::Device
  State::device(QString const& id) const
  {
    for (auto const& device: this->devices())
      if (device.id() == id)
        return device;
    elle::unreachable();
  }


  void
  State::login(std::string const& email,
               std::string const& password)
  {
    this->_login_future = QtConcurrent::run(
      [=] {
        auto res = gap_login(
          this->state(),
          email,
          password,
          boost::none, // Device push token.
          boost::none, // Country code.
          elle::system::platform::os_name(),
          QString_to_utf8_string(QHostInfo::localHostName())
        );
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
        auto res = gap_facebook_connect(
        this->state(),
          token,
          optional_email(),
          boost::none, // Device push token.
          boost::none, // Country code.
          elle::system::platform::os_name(),
          QString_to_utf8_string(QHostInfo::localHostName())
          );
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
        auto res = gap_register(
          this->state(),
          fullname,
          email,
          password,
          boost::none, // Device push token.
          boost::none, // Country code.
          elle::system::platform::os_name(),
          QString_to_utf8_string(QHostInfo::localHostName())
        );
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
      emit kicked_out(QString_from_utf8_string(last_error));
  }

  void
  State::on_contact_joined(uint32_t id,
                           std::string const& info)
  {
    ELLE_TRACE_SCOPE("%s: contact %s joined", *this, id);
    auto it = this->_users.get<0>().find(id);
    if (it == this->_users.get<0>().end())
    {
      auto p = this->_users.emplace(*this, id);
      it = p.first;
    }
    notification::center().notify(
      "Infinit",
      QString("Your contact %1 (%2) joined Infinit").arg(
        it->fullname(), QString_from_utf8_string(info)),
      2000);
  }

  std::vector<model::Device>
  State::devices() const
  {
    std::vector<model::Device> devices;
    std::vector<surface::gap::Device const*> _devices;
    auto res = gap_devices(this->state(), _devices);
    if (res != gap_ok)
      ELLE_WARN("%s: fetching devices failed", *this);
    else
      for (auto const& device: _devices)
        devices.emplace_back(*device);
    std::sort(devices.begin(), devices.end(), [] (model::Device const& l,
                                                  model::Device const& r)
              {
                return l.last_sync() > r.last_sync();
              });
    return devices;
  }

  struct UpdateTime
  {
    UpdateTime(model::Transaction const& transaction)
      : _transaction(transaction)
    {}

    void
    operator()(model::User& model)
    {
      model.last_interraction(this->_transaction.mtime());
    }

    ELLE_ATTRIBUTE(model::Transaction const&, transaction)
  };

  void
  State::use_ghost_code(std::string const& code,
                        bool manual)
  {
    ELLE_TRACE_SCOPE("use ghost code: %s", code);
    this->_ghost_code_future = QtConcurrent::run(
      [=] {
        return gap_use_ghost_code(this->state(), code, !manual);
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
    this->_logged_in = true;
    ELLE_TRACE("load swaggers")
    {
      std::vector<surface::gap::User> users;
      gap_Status res = gap_swaggers(this->state(), users);
      if (res != gap_ok)
        ELLE_ERR("getting swaggers failed %s", res);
      for (auto const& user: users)
      {
        auto p = this->_users.emplace(*this, user);
        if (!p.second)
          this->_users.replace(p.first, model::User(*this, user));
        ELLE_DEBUG("user: %s", this->user(user.id));
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
        auto const& tr = *this->_transactions.get<0>().find(transaction.id);
        ELLE_DEBUG("transaction: %s", tr);
        auto it = this->_users.get<0>().find(tr.peer().id());
        if (it != this->_users.get<0>().end())
          this->_users.modify(it, UpdateTime(tr));
      }
      this->_compute_active_transactions();
    }

    ELLE_TRACE("load links")
    {
      std::vector<surface::gap::LinkTransaction> links;
      gap_Status res = gap_link_transactions(this->state(), links);
      if (res != gap_ok)
        ELLE_ERR("getting links failed %s", res);
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
    gap_contact_joined_callback(
      this->state(),
      [this] (uint32_t user_id,
              std::string const& contact)
      {
        QApplication::postEvent(this, new ContactJoined(user_id, contact));
      });
    {
      std::string web_token;
      if (gap_web_login_token(this->state(), web_token) == gap_ok)
      {
        this->_web_token = QString_from_utf8_string(web_token);
      }
    }
    {
      this->_activate_reminders(true);
    }
  }

  void
  State::_activate_reminders(bool first)
  {
    auto create_reminder = [this, first] (uint32_t interval,
                                   bool loop)
      {
        auto* reminder =  new QTimer(this);
        ELLE_DEBUG_SCOPE("%s with interval %s", reminder, interval);
        reminder->setSingleShot(!loop);
        connect(reminder, SIGNAL(timeout()), this, SLOT(_transactions_reminder()));
        if (first)
          connect(reminder, SIGNAL(timeout()), this, SLOT(_activate_reminders()));
        reminder->setInterval(interval);
        reminder->start();
      };
    if (first)
    {
      auto initial = 10_min; // 10 min after the program starts.
      create_reminder(initial.total_milliseconds(), false);
    }
    else
    {
      auto r = 3_h; // 3hrs after.
      auto daily = 24_h; // Every 24hrs.
      create_reminder(r.total_milliseconds(), false);
      create_reminder(daily.total_milliseconds(), true);
    }
  }

  void
  State::_transactions_reminder()
  {
    ELLE_LOG_SCOPE("%s: transaction reminder (caller: %s)",
                     *this, QObject::sender());
    std::vector<model::Transaction const*> acceptables;
    for (auto const& tr: this->_transactions.get<0>())
    {
      if (tr.acceptable())
        acceptables.push_back(&tr);
    }
    if (acceptables.size() > 1)
      notification::center().notify(
        "Infinit",
        QString("You have %1 transactions waiting for you").arg(acceptables.size()),
        5000);
    else if (acceptables.size() == 1)
    {
      auto* notif = new fist::notification::IncomingTransaction(**acceptables.begin(), nullptr);
      connect(notif, SIGNAL(accept_transaction(uint32_t)),
              this, SLOT(on_transaction_accepted(uint32_t)));
      fist::notification::center().notify(notif);
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

  model::User&
  State::me()
  {
    ELLE_ASSERT(this->my_id() != gap_null());
    return this->user(this->my_id());
  }

  model::User const&
  State::me() const
  {
    ELLE_ASSERT(this->my_id() != gap_null());
    return this->user(this->my_id());
  }

  void
  State::_on_avatar_available(uint32_t id)
  {
    ELLE_TRACE_SCOPE("%s: avatar available for id %s", *this, id);
    auto* fetcher = new AvatarFetcher(this->user(id).id(), *this);
    connect(fetcher, SIGNAL(finished()), this, SLOT(_avatar_fetched()));
    fetcher->start();
  }

  void
  State::_avatar_fetched()
  {
    auto* fetcher = static_cast<AvatarFetcher*>(QObject::sender());
    elle::SafeFinally deleter([&] { fetcher->deleteLater(); });
    auto id = fetcher->id();
    {
      this->_avatar_mutex.lock();
      elle::SafeFinally unlock([&] { this->_avatar_mutex.unlock(); });
      ELLE_TRACE("got %s big avatar", fetcher->avatar().size());
      this->_avatars[id] = fetcher->avatar();
    }
    ELLE_TRACE("update %s avatar", this->user(id))
      this->user(id).avatar_available();
  }

  QByteArray&
  State::avatar(uint32_t id)
  {
    ELLE_DEBUG("get avatar for %s", id);
    {
      this->_avatar_mutex.lock();
      elle::SafeFinally unlock([&] { this->_avatar_mutex.unlock(); });
      if (this->_avatars.find(id) == this->_avatars.end())
      {
        this->_avatars[id] = QByteArray();
        if (id != gap_null())
        {
          auto* fetcher = new AvatarFetcher(id, *this);
          connect(fetcher, SIGNAL(finished()), this, SLOT(_avatar_fetched()));
          fetcher->start();
        }
      }
      return this->_avatars[id];
    }
  }

  void
  State::update_avatar(QPixmap const& avatar)
  {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    avatar.save(&buffer, "PNG");
    bool res = false;
    if (gap_update_avatar(this->state(),
                          reinterpret_cast<void const*>(bytes.constData()),
                          bytes.size()) == gap_ok)
    {
      this->_avatar_mutex.lock();
      elle::SafeFinally unlock([&] { this->_avatar_mutex.unlock(); });
      this->_avatars[this->my_id()] = bytes;
      res = true;
    }
    if (res)
      this->me().avatar_available();
  }

  void
  State::on_swagger_deleted(uint32_t id)
  {
    this->user(id).deleted(true);
  }

  void
  State::on_user_updated(surface::gap::User const& user)
  {
    ELLE_DEBUG_SCOPE("%s: peer %s updated", *this, user);
    auto it = this->_users.get<0>().find(user.id);
    if (it == this->_users.get<0>().end())
      this->_users.emplace(*this, user);
    else
      this->_users.replace(it, model::User(*this, user));
  }

  void
  State::on_user_status_changed(uint32_t id, bool status)
  {
    this->user(id).status(status);
  }

  State::Users
  State::swaggers(Filter const& filter)
  {
    ELLE_DEBUG_SCOPE("%s: get swaggers", *this);
    State::Users res;
    for (auto const& user: boost::adaptors::reverse(this->_users.get<1>()))
    {
      auto ignore = !user.swagger() && !user.deleted();
      if (!ignore && filter(user))
      {
        ELLE_DEBUG("add result: %s", user)
          res.push_back(user.id());
      }
    }
    return res;
  }

  // Return a subset of the swaggers.
  State::Users
  State::swaggers(QString const& filter)
  {
    ELLE_TRACE_SCOPE("%s: get swaggers matching %s", *this, filter);
    return this->swaggers(
      [&filter] (model::User const& user) -> bool
      {
        return filter.isEmpty()
          ? true
          : (user.fullname().toLower().contains(filter.toLower()) ||
             user.handle().toLower().contains(filter.toLower()) ||
             user.emails().contains(filter.toLower()));
      });
  }

  State::Users
  State::search(QString const& filter)
  {
    ELLE_DEBUG("search %s", filter);
    ELLE_DEBUG("cancel future")
      this->cancel_search();
    auto results = this->swaggers(filter);
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
              {
                auto& user = State::user(u.id);
                user.add_email(filter);
                if (!user.deleted())
                  users.push_back(user.id());
              }
              else
                ELLE_WARN("user by email failed: %s", res);
            }
            return users;
          });
      this->_search_watcher.setFuture(this->_search_future);
    }
    return results;
  }

  uint32_t
  State::user_id(std::string const& email)
  {
    surface::gap::User u;
    auto res = gap_user_by_email(this->state(), email.c_str(), u);
    if (res == gap_ok)
    {
      return this->user(u.id).id();
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
  State::user(uint32_t user_id) const
  {
    if (this->_users.get<0>().find(user_id) == this->_users.get<0>().end())
      this->_users.emplace(const_cast<State&>(*this), user_id);
    return *this->_users.get<0>().find(user_id);
  }

  model::User&
  State::user(uint32_t user_id)
  {
    if (this->_users.get<0>().find(user_id) == this->_users.get<0>().end())
        this->_users.emplace(*this, user_id);
    return const_cast<model::User&>(*this->_users.get<0>().find(user_id));
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
      emit search_results_ready();
    }
    else
    {
      ELLE_DEBUG("future empty");
    }
  }

  QString
  State::download_folder() const
  {
    return QString_from_utf8_string(gap_get_output_dir(this->state()));
  }

  void
  State::download_folder(QString const& folder)
  {
    auto _folder = QString_to_utf8_string(folder);
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
      UpdateStatus(surface::gap::PeerTransaction const& transaction,
                   bool& changed)
        : _transaction(transaction)
        , _changed(changed)
      {}

      UpdateStatus(UpdateStatus const& updater)
        : _transaction(updater._transaction)
        , _changed(updater._changed)
      {
      }

      void
      operator()(model::Transaction& model)
      {
        this->_changed = model.transaction(this->_transaction);
      }

      ELLE_ATTRIBUTE(surface::gap::PeerTransaction, transaction)
      ELLE_ATTRIBUTE(bool&, changed);
    };

    bool changed = false;
    auto const& updater = UpdateStatus(tr, changed);
    this->_transactions.modify(it, updater);
    if (changed)
      emit transaction_updated(id);
    {
      auto const& tr = *it;
      auto peer = this->_users.get<0>().find(tr.peer().id());
      if (peer != this->_users.get<0>().end())
        this->_users.modify(peer, UpdateTime(tr));
    }
    this->_compute_active_transactions();
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
    this->_compute_active_links();
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
  State::on_transaction_pause_toggled(uint32_t id)
  {
    ELLE_ASSERT(id != gap_null());
    gap_toggle_transaction_pause(
      this->state(), id, const_cast<model::Transaction&>(*this->_transactions.get<0>().find(id)).pause());
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
    auto id = QString_from_utf8_string(gap_facebook_app_id());
    return id;
  }

  void
  State::account(Account const& account)
  {
    auto old_plan = this->_account.plan.value();
    auto new_plan = account.plan.value();
    this->_account = account;
    ELLE_DEBUG_SCOPE("plan updated: %s -> %s (notify: %s) (%s)",
                     old_plan, new_plan, this->_logged_in,
                     account);
    if (this->_logged_in && new_plan != old_plan)
    {
      notification::center().notify(
        "Infinit",
        QString("Your plan has been %1graded to %2").arg(
          QString(new_plan > old_plan ? "up": "down"),
          QString_from_utf8_string(elle::sprintf("%s", new_plan))));
    }
    emit account_updated();
  }

  QUrl
  State::profile_url(QString const& utm_campaign) const
  {
    QString _url = QString("https://infinit.io/account?login_token=%1&email=%2").arg(
      this->web_token(),
      url_encode(this->me().emails()[0]));
    QString url = _url;
    if (!utm_campaign.isEmpty())
    {
      url.append(QString("&utm_source=app&utm_medium=windows&utm_campaign=%1").arg(utm_campaign));
    }
    ELLE_DEBUG("profile url: %s", url);
    return QUrl(url, QUrl::StrictMode);
  }

  void
  State::go_to_online_profile(QString const& utm_campaign) const
  {
    QDesktopServices::openUrl(this->profile_url(utm_campaign));
  }

  bool
  State::update_fullscreen_screenshot_shortcut(QString const& sequence_)
  {
    QString sequence = sequence_;
    if (sequence.isEmpty())
      sequence = fist::settings()["Shortcuts"].get("fullscreen",
                                                   "Ctrl+Shift+I").toString();
    return this->fullscreen_screenshot->setShortcut(QKeySequence(sequence));
  }

  bool
  State::update_region_screenshot_shortcut(QString const& sequence_)
  {
    QString sequence = sequence_;
    if (sequence.isEmpty())
      sequence = fist::settings()["Shortcuts"].get("region",
                                                   "Ctrl+Shift+O").toString();
    return this->region_screenshot->setShortcut(QKeySequence(sequence));
  }

  void
  AvatarFetcher::run()
  {
    /// Get user icon data.
    void* data = nullptr;
    size_t len = 0;
    auto res = gap_avatar(this->_state.state(), this->id(), &data, &len);
    if (res == gap_ok)
    {
      if (len > 0) // An avatar is avalaible. If not, keep the default.
      {
        ELLE_DEBUG("%s: get avatar data", *this);
        QByteArray raw((char *) data, len);
        this->_avatar = raw;
      }
    }
  }

  void
  State::customEvent(QEvent* event)
  {
    elle::SafeFinally acceptor([&] { event->accept(); });
    if (event->type() == CONTACT_JOINED)
    {
      auto* contact_joined = static_cast<ContactJoined*>(event);
      this->on_contact_joined(contact_joined->id(),
                              contact_joined->contact());
    }
    else if (event->type() == ACCOUNT_UPDATED)
    {
      auto* account_updated = static_cast<AccountUpdated*>(event);
      this->account(account_updated->account());
    }
    else
    {
      acceptor.abort();
    }
  }
}
