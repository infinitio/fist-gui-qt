#include <QApplication>
#include <QHBoxLayout>
#include <QByteArray>
#include <QPixmap>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QToolTip>
#include <QVBoxLayout>
#include <QFuture>
#include <QtConcurrentRun>

#include <elle/Buffer.hh>
#include <elle/finally.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/log.hh>

#include <cryptography/SecretKey.hh>
#include <cryptography/Exception.hh>

#include <surface/gap/gap.hh>
#include <version.hh>

#include <fist-gui-qt/login/Window.hh>
#include <fist-gui-qt/login/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/icons.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/Settings.hh>

ELLE_LOG_COMPONENT("infinit.FIST.login.Window");

namespace fist
{
  namespace login
  {
    static QRegExp email_checker(regexp::email,
                                 Qt::CaseInsensitive);
    static
    infinit::cryptography::SecretKey
    secret_key(QString const& email)
    {
      static std::string secret_key_password("S4LT%sPasswordSecretKey");
      return infinit::cryptography::SecretKey(
        infinit::cryptography::cipher::Algorithm::aes256,
        elle::sprintf(secret_key_password, email.toStdString()));
    }

    void
    Window::_save_password(QString const& email,
                           QString const& password)
    {
      auto encrypted_password = secret_key(email).encrypt(
        password.toStdString());
      std::string encrypted_password_string;
      elle::serialize::to_string(encrypted_password_string) << encrypted_password;
      auto eps = encrypted_password_string;
      QByteArray ar(eps.data(), eps.length());
      fist::settings()["Login"].set("password", ar);
    }

    QString
    Window::_saved_password(QString const& email) const
    {
      auto saved_password =
        fist::settings()["Login"].get("password", "").toByteArray();
      if (!saved_password.isEmpty())
      {
        try
        {
          std::string str(saved_password.constData(), saved_password.size());
          infinit::cryptography::Code code;
          elle::serialize::from_string(str) >> code;
          auto password_string = secret_key(email).decrypt<std::string>(code);
          return QString::fromStdString(password_string);
        }
        catch (elle::Exception const& e)
        {
          ELLE_WARN("%s: enable to deserialize stored password: %s",
                    *this, e.what());
          fist::settings()["Login"].remove("password");
        }
      }
      return QString();
    }

    Window::Window(fist::State& state,
                     fist::gui::systray::Icon& systray,
                     bool fill_email_and_password_fields,
                     bool previous_session_crashed):
      RoundShadowWidget(0, 0, Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
      _state(state),
      _previous_session_crashed(previous_session_crashed),
      _systray(systray),
      _mode(Mode::None),
      _loading_icon(new QMovie(QString(":/loading"), QByteArray(), this)),
      _loading(new QLabel(this)),
      _info(new QLabel(view::login::info::login_text, this)),
      _fullname_field(new QLineEdit(this)),
      _email_field(new QLineEdit(this)),
      _password_field(new QLineEdit(this)),
      _message_field(new QLabel(this)),
      _help_link(new QLabel(view::login::links::forgot_password::text, this)),
      _switch_mode(new QLabel(this)),
      _version_field(new QLabel(this)),
      _footer(new Footer(this)),
      _login_future(),
      _login_watcher()
    {
      ELLE_TRACE_SCOPE("%s: contruction", *this);
      this->setWindowIcon(QIcon(":/logo"));
      this->resize(view::login::size);
      {
        auto saved_email = fist::settings()["Login"].get("email", "").toString();
        // Info
        {
          this->_loading->setMovie(this->_loading_icon);
          this->_loading->movie()->start();
          this->_loading->hide();
        }
        // Info
        {
          this->_info->setFixedSize(view::login::info::size);
          view::login::info::style(*this->_info);
        }
        // Fullname field.
        {
          this->_fullname_field->setPlaceholderText(view::login::fullname::placeholder);
          this->_fullname_field->setFixedSize(view::login::fullname::size);
          view::login::fullname::style(*this->_fullname_field);
          this->_fullname_field->setTextMargins(12, 0, 12, 0);
        }
        // Email field.
        {
          this->_email_field->setPlaceholderText(view::login::email::placeholder);
          this->_email_field->setFixedSize(view::login::email::size);
          view::login::email::style(*this->_email_field);
          this->_email_field->setTextMargins(12, 0, 12, 0);

          if (fill_email_and_password_fields && !saved_email.isEmpty())
            this->_email_field->setText(saved_email);
        }
        // Password field.
        {
          this->_password_field->setPlaceholderText(view::login::password::placeholder);
          this->_password_field->setFixedSize(view::login::password::size);
          view::login::password::style(*this->_password_field);
          this->_password_field->setTextMargins(12, 0, 12, 0);
          this->_password_field->setEchoMode(QLineEdit::Password);
          if (fill_email_and_password_fields && !saved_email.isEmpty())
          {
            this->_password_field->setText(this->_saved_password(saved_email));
          }
        }
      }
      if (!this->_email_field->text().isEmpty())
        this->_password_field->setFocus();
      // Logo.
      auto logo = new QLabel;
      {
        logo->setScaledContents(true);
        logo->setPixmap(QPixmap(":/login/logo"));
      }
      // Message field.
      {
        view::login::message::error_style(*this->_message_field);
        this->_message_field->setWordWrap(true);
        this->_message_field->setFixedWidth(this->width() - 10);
      }
      // Create account.
      {
        view::login::links::style(*this->_switch_mode);
        this->_switch_mode->installEventFilter(this);
        this->_switch_mode->setSizePolicy(
          QSizePolicy::Minimum, QSizePolicy::Maximum);
      }
      // Forgotten password.
      {
        view::login::links::style(*this->_help_link);
        this->_help_link->setTextInteractionFlags(
          view::login::links::interration_flags);
        this->_help_link->setSizePolicy(
          QSizePolicy::Minimum, QSizePolicy::Maximum);
        this->_help_link->setOpenExternalLinks(true);
      }
      // Version.
      {
        view::login::version::style(*this->_version_field);
        this->_version_field->hide();
      }
      // Footer.
      {
        connect(this->_footer, SIGNAL(login()),
                this, SLOT(_login()));
        connect(this->_footer, SIGNAL(register_()),
                this, SLOT(_register()));
      }
      auto central_widget = new QWidget(this);
      {
        central_widget->setFixedWidth(this->width());
        // central_widget->setMinimumHeight(300);
      }
      auto layout = new QVBoxLayout(central_widget);
      layout->setSizeConstraint(QLayout::SetFixedSize);
      layout->setSpacing(5);
      layout->setContentsMargins(0, 2, 0, 0);
      {
        auto hlayout = new QHBoxLayout();
        hlayout->setSizeConstraint(QLayout::SetFixedSize);
        hlayout->setContentsMargins(7, 0, 7, 0);
        hlayout->addWidget(this->_version_field, 0, Qt::AlignCenter);
        hlayout->addStretch();
        layout->addLayout(hlayout);
      }
      layout->addSpacing(10);
      layout->addWidget(logo, 0, Qt::AlignCenter);
      layout->addSpacing(20);
      layout->addStretch();
      layout->addWidget(this->_loading, 0, Qt::AlignCenter);
      layout->addStretch();
      layout->addWidget(this->_message_field, 0, Qt::AlignCenter);
      layout->addStretch();
      layout->addSpacing(15);
      layout->addWidget(this->_info, 0, Qt::AlignCenter);
      layout->addWidget(this->_fullname_field, 0, Qt::AlignCenter);
      layout->addWidget(this->_email_field, 0, Qt::AlignCenter);
      layout->addWidget(this->_password_field, 0,  Qt::AlignCenter);
      layout->addSpacing(1);
      {
        auto hlayout = new QHBoxLayout();
        hlayout->addSpacing(40);
        hlayout->addWidget(this->_switch_mode, 0, Qt::AlignRight);
        hlayout->addStretch();
        hlayout->addWidget(this->_help_link, 0, Qt::AlignLeft);
        hlayout->addSpacing(40);
        layout->addLayout(hlayout, Qt::AlignCenter);
      }
      layout->addStretch();
      layout->addSpacing(25);
      layout->addWidget(this->_footer);
      this->setCentralWidget(central_widget);
      connect(&this->_login_watcher, SIGNAL(finished()),
              this, SLOT(_login_attempt()));
      connect(this, SIGNAL(logged_in()), &this->_state, SLOT(on_logged_in()));
      connect(this, SIGNAL(login_failed()), SLOT(show()));
      connect(&this->_register_watcher, SIGNAL(finished()),
              this, SLOT(_register_attempt()));
      connect(&this->_state, SIGNAL(internet_issue(QString const&)),
              this, SLOT(_internet_issue(QString const&)));
      this->update();

      if (this->_email_field->text().isEmpty())
      {
        this->mode(Mode::Register);
        this->show();
      }
      else
      {
        this->mode(Mode::Login);
        if (!this->_previous_session_crashed)
        {
          this->_systray.show();
          this->try_auto_login();
        }
        else
        {
          this->show();
          this->set_message(
            "<a>"
              "Previous session crashed<br>"
              "Feel free to contact us at "
              "<a "
                " style=\"text-decoration: none; color: #489FCE;\""
                " href=\"mailto:contact@infinit.io\">"
                  "contact@infinit.io"
              "</a>"
            "</a>",
            "Previous session crashed.",
            false);
        }
      }
    }

    Window::~Window()
    {
      ELLE_TRACE_SCOPE("%s: destruction", *this);
    }

    void
    Window::mode(fist::login::Mode mode)
    {
      ELLE_TRACE_SCOPE("Set mode %s", mode);
      this->_mode = mode;
      this->_message_field->clear();
      this->_footer->mode(mode);
      this->_loading->hide();
      switch (mode)
      {
        case Mode::Register:
          this->_switch_mode->setText(
            view::login::links::already_have_an_account::text);
          this->_help_link->setText(
            view::login::links::help::text);
          this->_info->setText(
            view::login::info::register_text);
          this->_fullname_field->show();
          this->_footer->setFocus();
          break;
        case Mode::Login:
          this->_switch_mode->setText(
            view::login::links::need_an_account::text);
          this->_help_link->setText(
            view::login::links::forgot_password::text);
          this->_info->setText(
            view::login::info::login_text);
          this->_fullname_field->hide();
          this->_email_field->setFocus();
          break;
        case Mode::Loading:
          this->_loading->show();
          break;
        default:
          ;
      }
    }

    bool
    Window::eventFilter(QObject *obj, QEvent *event)
    {
      if (!dynamic_cast<QWidget*>(obj) &&
          !dynamic_cast<QLayout*>(obj))
        return Super::eventFilter(obj, event);

      if (obj == this->_switch_mode)
      {
        if (event->type() == QEvent::MouseButtonRelease)
        {
          ELLE_TRACE("change mode");
          if (this->_mode == Mode::Login)
            this->mode(Mode::Register);
          else if (this->_mode == Mode::Register)
            this->mode(Mode::Login);
        }
      }
      return Super::eventFilter(obj, event);
    }

    void
    Window::_register_attempt()
    {
      ELLE_TRACE_SCOPE("%s: attempt to register", *this);
      this->mode(Mode::Register);
      elle::SafeFinally unlock_register([&] {
          this->_enable(); this->_password_field->setFocus(); });
      auto status = this->_register_future.result();
      if (status == gap_ok)
      {
        auto email = this->_email_field->text();
        auto password = this->_password_field->text();
        fist::settings()["Login"].set("email", email);
        this->_save_password(email, password);
        emit logged_in();
        return;
      }
      ELLE_TRACE("register failed");
      emit this->register_failed();
      if (status == gap_deprecated)
      {
        emit version_rejected();
      }
      switch (status)
      {
#define ERR(error_code, msg, comp_to_focus)                   \
        case error_code:                                      \
          this->set_message(msg);                             \
          if (comp_to_focus != nullptr)                       \
          {                                                   \
            static_cast<QWidget*>(comp_to_focus)->setFocus(); \
          }                                                   \
          break                                               \
            /**/
        ERR(gap_network_error, "No connection to the Internet", nullptr);
        ERR(gap_meta_unreachable, "Unable to contact Infinit server", nullptr);
        ERR(gap_meta_down_with_message, "Infinit server unavailable", nullptr);
        ERR(gap_trophonius_unreachable, "Unable to contact Infinit server", nullptr);
        ERR(gap_email_not_valid, "Email not valid", this->_email_field);
        ERR(gap_fullname_not_valid, "Fullname not valid", this->_fullname_field);
        // Handle is generated automatically from fullname...
        ERR(gap_handle_already_registered, "Fullname not valid", this->_fullname_field);
        ERR(gap_handle_not_valid, "Fullname not valid", this->_fullname_field);
        ERR(gap_email_already_registered, "This email has already been taken", this->_email_field);
        ERR(gap_deprecated, "Your version is no longer supported", nullptr);
        default:
          this->set_message(QString::fromStdString(elle::sprintf("Internal error (%s)", status)));
      }
#undef ERR
    }

    void
    Window::_login_attempt()
    {
      this->mode(Mode::Login);
      ELLE_TRACE_SCOPE("%s: attempt to login", *this);
      elle::SafeFinally unlock_login([&] {
          this->_enable(); this->_password_field->setFocus(); });
      auto status = this->_login_future.result();
      if (status == gap_ok || status == gap_already_logged_in)
      {
        emit logged_in();
        auto email = this->_email_field->text();
        auto password = this->_password_field->text();
        fist::settings()["Login"].set("email", email);
        this->_save_password(email, password);
        return;
      }
      ELLE_TRACE("login failed");
      emit this->login_failed();
      if (status == gap_deprecated)
      {
        emit version_rejected();
      }
      static auto fill_error_field = [&] (std::string const& error_message,
                                          std::string const& sub_message = "")
        {
          ELLE_WARN("%s", error_message);
          this->set_message(error_message.c_str(), sub_message.c_str());
        };
      switch (status)
      {
#define ERR(error_code, msg)                    \
        case error_code:                        \
          fill_error_field(msg);                \
          break                                 \
            /**/
        ERR(gap_email_not_valid, "Email not valid");
        ERR(gap_network_error, "No connection to the Internet");
        ERR(gap_meta_unreachable, "Unable to contact Infinit server");
        ERR(gap_meta_down_with_message, "Infinit server unavailable");
        ERR(gap_trophonius_unreachable, "Unable to contact Infinit server");
        ERR(gap_email_password_dont_match, "Wrong email/password");
        ERR(gap_deprecated, "Your version is no longer supported");
        ERR(gap_email_not_confirmed, "You need to confirm your email\nCheck your inbox");
        default:
          fill_error_field(elle::sprintf("Internal error (%s)", status));
      }
#undef ERR
    }

    void
    Window::_enable()
    {
      this->_footer->setDisabled(false);
      this->_switch_mode->setDisabled(false);
      this->_email_field->setDisabled(false);
      this->_password_field->setDisabled(false);
      this->_fullname_field->setDisabled(false);
    }

    void
    Window::_disable(bool disable_fullname)
    {
      this->_switch_mode->setDisabled(true);
      this->_footer->setDisabled(true);
      this->_email_field->setDisabled(true);
      this->_password_field->setDisabled(true);
      if (disable_fullname)
        this->_fullname_field->setDisabled(true);
    }

    void
    Window::try_auto_login()
    {
      ELLE_TRACE_SCOPE("%s: try auto login", *this);
      if (!this->_password_field->text().isEmpty())
      {
        this->hide();
        this->_login();
      }
      else
      {
        this->show();
      }
    }

    void
    Window::_internet_issue(QString const& reason)
    {
      ELLE_WARN("%s: something went wrong while logging in: %s", *this, reason);
      this->set_message(reason, reason, false);
    }

    bool
    Window::_test_fields(bool test_fullname)
    {
      if (test_fullname && this->_fullname_field->text().isEmpty())
      {
        ELLE_DEBUG("invalid fullname field");
        this->set_message("You need a name",
                          "It will make you easier to find for your contacts");
        this->_fullname_field->setDisabled(false);
        this->_fullname_field->setFocus();
        return false;
      }
      if (this->_email_field->text().isEmpty() ||
          !email_checker.exactMatch(this->_email_field->text()))
      {
        ELLE_DEBUG("invalid email field");
        if (this->_email_field->text().isEmpty())
          this->set_message("You must choose an email address",
                            "We need your email, sorry");
        else
          this->set_message("This email address seems invalid",
                            "We need your email, sorry");
        this->_email_field->setDisabled(false);
        this->_email_field->setFocus();
        return false;
      }
      if (this->_password_field->text().isEmpty())
      {
        ELLE_DEBUG("no password");
        this->set_message("Choose a password",
                          "It will be stored locally, not a big deal if you don't remember");
        this->_password_field->setDisabled(false);
        this->_password_field->setFocus();
        return false;
      }
      return true;
    }

    void
    Window::_register()
    {
      elle::SafeFinally unlock_register([&] {
          this->_enable();
          this->_footer->mode(Mode::Register);
        });
      this->_disable(true);
      elle::SafeFinally register_failed_guard([&] { emit this->register_failed(); });
      this->_message_field->clear();
      if (!this->_test_fields(true))
        return;
      QString email = this->_email_field->text();
      QString pw = this->_password_field->text();
      QString fullname = this->_fullname_field->text();
      register_failed_guard.abort();
      ELLE_TRACE("every check passed")
      {
        emit this->register_attempt();
        this->_register_future = QtConcurrent::run(
          [=] {
            // Will explode if the state is destroyed.
            std::string hash{
              gap_hash_password(
                this->_state.state(),
                email.toStdString().c_str(),
                pw.toStdString().c_str())};
            return gap_register(
              this->_state.state(),
              fullname.toStdString().c_str(),
              email.toStdString().c_str(),
              hash.c_str());
          });
      }
      this->_register_watcher.setFuture(this->_register_future);
      unlock_register.abort();
      this->mode(Mode::Loading);
    }

    void
    Window::_login()
    {
      elle::SafeFinally unlock_login([&] {
          this->_enable();
          this->_footer->mode(Mode::Login);
        });
      this->_disable();
      elle::SafeFinally login_failed_guard([&] { emit this->login_failed(); });
      this->_message_field->clear();
      ELLE_TRACE_SCOPE("%s: login attempt", *this);
      if (!this->_test_fields())
        return;
      QString email = this->_email_field->text();
      QString pw = this->_password_field->text();
      login_failed_guard.abort();
      ELLE_TRACE("every check passed")
      {
        emit this->login_attempt();
        this->_login_future = QtConcurrent::run(
          [=] {
            // Will explode if the state is destroyed.
            char* hash = gap_hash_password(
              this->_state.state(), email.toStdString().c_str(), pw.toStdString().c_str());

            elle::SafeFinally free_hash([&] { gap_hash_free(hash); });
            return gap_login(this->_state.state(), email.toStdString().c_str(), hash);
          });
      }
      this->_login_watcher.setFuture(this->_login_future);
      unlock_login.abort();
      this->mode(Mode::Loading);
    }

    void
    Window::set_message(QString const& message,
                             QString const& tooltip,
                             bool critical)
    {
      ELLE_TRACE_SCOPE("%s: set message (%s)", *this, message);
      if (critical)
        view::login::message::error_style(*this->_message_field);
      else
        view::login::message::warning_style(*this->_message_field);
      this->_message_field->setText(message);
      this->_message_field->setToolTip(tooltip);
    }

    void
    Window::set_version()
    {
      ELLE_TRACE_SCOPE("%s: set version: %s", *this, INFINIT_VERSION);
      this->_version_field->setText(
        QString::fromStdString(
          elle::sprintf("v%s", INFINIT_VERSION)));
      this->_version_field->show();
      this->update();
    }

    void
    Window::update_available(bool mandatory,
                                  QString const& changelog)
    {
      ELLE_TRACE_SCOPE("%s: update available", *this);
      if (this->_previous_session_crashed)
        this->set_message("Previous session crashed and an update is available!\n"
                          "We recommand you to wait for the update!",
                          "Previous session crashed and an update is available!\n"
                          "We recommand you to wait for the update!",
                          false);
    }

    void
    Window::download_progress(qint64 downloaded, qint64 total_size)
    {
      ELLE_DUMP("progress: %s", 100 * downloaded / total_size);
    }

    void
    Window::download_ready()
    {
      ELLE_TRACE_SCOPE("%s: download ready", *this);
      emit update_application();
    }

    void
    Window::keyPressEvent(QKeyEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: key pressed (%s)", *this, event->key());

      if (event->key() == Qt::Key_Escape)
        this->_reduce();
      else if (event->key() == Qt::Key_Return)
        emit this->_footer->click();
    }

    void
    Window::focusInEvent(QFocusEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: gain focus", *this);

      Super::focusInEvent(event);

      if (this->_fullname_field->text().isEmpty() &&
          this->_mode == Mode::Register)
      {
        ELLE_DEBUG("focus fullname field");
        this->_fullname_field->setFocus();
      }
      else if (this->_email_field->text().isEmpty() ||
          !email_checker.exactMatch(this->_email_field->text()))
      {
        ELLE_DEBUG("focus email field");
        this->_email_field->setFocus();
      }
      else
      {
        ELLE_DEBUG("focus password field");
        this->_password_field->setFocus();
      }
    }

    void
    Window::_reduce()
    {
      ELLE_TRACE_SCOPE("%s: reduce", *this);

      this->setWindowState(Qt::WindowMinimized);
    }

    void
    Window::print(std::ostream& stream) const
    {
      stream << "Window";
    }

    void
    Window::closeEvent(QCloseEvent* event)
    {
      ELLE_TRACE_SCOPE("%s: close", *this);

      Super::closeEvent(event);

      emit quit_request();
    }

    void
    Window::showEvent(QShowEvent* event)
    {
      Super::showEvent(event);
      this->_systray.hide();
    }

    void
    Window::hideEvent(QHideEvent* event)
    {
      Super::hideEvent(event);
      this->_systray.show();
    }

  }
}
