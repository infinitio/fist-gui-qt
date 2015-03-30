#include <iostream>

#include <QApplication>
#include <QByteArray>
#include <QDesktopWidget>
#include <QFuture>
#include <QHBoxLayout>
#include <QPixmap>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QToolTip>
#include <QVBoxLayout>
#include <QPainter>
#include <QBrush>

#include <elle/Buffer.hh>
#include <elle/finally.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/log.hh>

#include <cryptography/SecretKey.hh>
#include <cryptography/Exception.hh>

#include <surface/gap/gap.hh>
#include <version.hh>

#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/icons.hh>
#include <fist-gui-qt/login/Window.hh>
#include <fist-gui-qt/login/facebook/ConnectWindow.hh>
#include <fist-gui-qt/login/ui.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/regexp.hh>

ELLE_LOG_COMPONENT("infinit.FIST.login.Window");

#ifndef INFINIT_WINDOWS
// # define VIDEO
#endif

#ifdef VIDEO
static Phonon::VideoPlayer* player;
#endif

namespace fist
{
  namespace login
  {
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
      RoundShadowWidget(
        0, 0, Qt::Window | Qt::WindowCloseButtonHint |
              Qt::WindowMinimizeButtonHint |
              Qt::MSWindowsFixedSizeDialogHint),
      _state(state),
      _previous_session_crashed(previous_session_crashed),
      _systray(systray),
      _mode(Mode::None),
      _signup_tabber(new QLabel(view::mode::signup::text, this)),
      _login_tabber(new QLabel(view::mode::login::text, this)),
      _separator(new QLabel(view::separator::text, this)),
      _loading_icon(new QMovie(QString(":/loading"), QByteArray(), this)),
      _loading(new QLabel(this)),
      _facebook_email_info(new QWidget(this)),
      _fullname_field(new QLineEdit(this)),
      _email_field(new QLineEdit(this)),
      _password_field(new QLineEdit(this)),
      _message_field(new QLabel(this)),
      _help_link(new QLabel(view::links::help::text, this)),
      _forgot_password_link(new QLabel(view::links::forgot_password::text, this->_password_field)),
      _version_field(new QLabel(this)),
      _login_button(new QPushButton(this)),
      _facebook_button(new QPushButton(this)),
      _video(nullptr),
      _facebook_window(nullptr),
      _facebook_connect_attempt(false)
    {
      ELLE_TRACE_SCOPE("%s: contruction", *this);
      this->setWindowIcon(QIcon(":/logo"));
      this->resize(view::size);
      {
        auto saved_email = fist::settings()["Login"].get("email", "").toString();
        // Help
        {
          this->_help_link->adjustSize();
        }
        // Loading.
        {
          this->_loading->setMovie(this->_loading_icon);
          this->_loading->movie()->start();
          this->_loading->hide();
        }
        // Mode.
        {
          view::mode::default_style(*this->_signup_tabber);
          view::mode::default_style(*this->_login_tabber);
        }
        // Separator.
        {
          view::separator::style(*this->_separator);
        }
        // facebook_email_info.
        {
          this->_facebook_email_info->setFixedWidth(view::password::size.width());
          QVBoxLayout* ll = new QVBoxLayout(this->_facebook_email_info);
          auto* title = new QLabel(view::facebook_email::title::text, this);
          view::facebook_email::title::style(*title);
          ll->addWidget(title);

          auto* subtitle =
            new QLabel(view::facebook_email::subtitle::text, this);
          view::facebook_email::subtitle::style(*subtitle);
          ll->addWidget(subtitle);
        }
        // Fullname field.
        {
          this->_fullname_field->setPlaceholderText(view::fullname::placeholder);
          this->_fullname_field->setFixedSize(view::fullname::size);
          view::fullname::style(*this->_fullname_field);
          this->_fullname_field->setTextMargins(12, 0, 12, 0);
        }
        // Email field.
        {
          this->_email_field->setPlaceholderText(view::email::placeholder);
          this->_email_field->setFixedSize(view::email::size);
          view::email::style(*this->_email_field);
          this->_email_field->setTextMargins(12, 0, 12, 0);

          if (fill_email_and_password_fields && !saved_email.isEmpty())
            this->_email_field->setText(saved_email);
        }
        // Password field.
        {
          this->_password_field->setPlaceholderText(view::password::placeholder);
          this->_password_field->setFixedSize(view::password::size);
          view::password::style(*this->_password_field);
          this->_password_field->setTextMargins(12, 0, 12, 0);
          this->_password_field->setEchoMode(QLineEdit::Password);
          if (fill_email_and_password_fields && !saved_email.isEmpty())
          {
            this->_password_field->setText(this->_saved_password(saved_email));
          }

          this->_forgot_password_link->setText(view::links::forgot_password::text);
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
        view::message::error_style(*this->_message_field);
        this->_message_field->setWordWrap(true);
        this->_message_field->setFixedWidth(this->width() - 10);
      }
      // Create account.
      {
        this->_login_tabber->installEventFilter(this);
        this->_login_tabber->setSizePolicy(
          QSizePolicy::Minimum, QSizePolicy::Maximum);
        this->_signup_tabber->installEventFilter(this);
        this->_signup_tabber->setSizePolicy(
          QSizePolicy::Minimum, QSizePolicy::Maximum);
      }
      // Help.
      {
        for (auto& link: {this->_help_link, this->_forgot_password_link})
        {
          view::links::style(*link);
          link->setTextInteractionFlags(view::links::interration_flags);
          link->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
          link->setOpenExternalLinks(true);
        }
      }
      // Version.
      {
        view::version::style(*this->_version_field);
        this->_version_field->hide();
      }
      // LoginButton.
      {
        this->_login_button->setFixedSize(view::login_button::size);
        view::login_button::style(*this->_login_button);
        this->_login_button->setStyleSheet(
          QString::fromStdString(elle::sprintf(
          "QPushButton {"
          "  background-color: rgb(248, 93, 91);"
          "  color: white;"
          "  font: bold 13px;"
          "  border-radius: 3px;"
          "  width: %spx;"
          "  height: %spx;"
          "} ",
          view::login_button::size.width(),
          view::login_button::size.height())));
      }
      // FacebookButton.
      {
        this->_facebook_button->setIcon(QIcon(":/login/facebook"));
        this->_facebook_button->setText("SIGN IN WITH FACEBOOK");
        this->_facebook_button->setStyleSheet(
          QString::fromStdString(elle::sprintf(
            "QPushButton {"
            "  background-color: rgb(77, 117, 210);"
            "  border-radius: 3px;"
            "  color: white;"
            "  font: bold 13px;"
            "  width: %spx;"
            "  height: %spx;"
            "} ",
            view::login_button::size.width(),
            view::login_button::size.height())));
        connect(this->_facebook_button, SIGNAL(released()),
                this, SLOT(launch_facebook_connect()));
      }
      // Video Logo
      {
 #ifdef VIDEO
        player = new Phonon::VideoPlayer(Phonon::VideoCategory, this);
        player->play(Phonon::MediaSource(":/login/preboardingideo.avi"));
        player->pause();
        auto* widget = player->videoWidget();
        widget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
        widget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
        widget->setFixedSize(400, 470);
        this->_video = widget;
#else
        auto* widget = new QLabel(this);
        widget->setMovie(new QMovie(":/login/preboarding"));
        widget->movie()->start();
        // widget->setScaledContents(true);
        this->_video = widget;
        this->_update_geometry();
#endif
        // widget->setFixedSize(400, 470);
        widget->show();
        widget->installEventFilter(this);
      }
      auto central_widget = new QWidget(this);
      {
        // central_widget->setFixedWidth(this->width());
      }
      auto glayout = new QHBoxLayout(central_widget);
      glayout->setContentsMargins(0, 0, 0, 0);
      glayout->setSpacing(0);
      auto mlayout = new QVBoxLayout;
      mlayout->setSizeConstraint(QLayout::SetFixedSize);
      mlayout->setSpacing(0);
      mlayout->setContentsMargins(0, 5, 5, 0);
      mlayout->addWidget(this->_help_link, 0, Qt::AlignRight);
      auto layout = new QVBoxLayout;
      mlayout->addLayout(layout);
      layout->setSpacing(5);
      layout->setContentsMargins(40, 0, 40, 0);
      {
        layout->addSpacing(5);
        layout->addWidget(logo, 0, Qt::AlignCenter);
        layout->addSpacing(10);
        {
          auto hlayout = new QHBoxLayout();
          hlayout->addStretch();
          hlayout->addWidget(this->_signup_tabber);
          hlayout->addSpacing(15);
          hlayout->addWidget(this->_loading);
          hlayout->addSpacing(15);
          hlayout->addWidget(this->_login_tabber);
          hlayout->addStretch();
          layout->addLayout(hlayout);
        }
      }
      layout->addSpacing(35);
      layout->addWidget(this->_facebook_button, 0, Qt::AlignCenter);
      layout->addSpacing(5);
      layout->addWidget(this->_separator, 0, Qt::AlignCenter);
      layout->addSpacing(5);
      layout->addWidget(this->_facebook_email_info, 0, Qt::AlignCenter);
      layout->addWidget(this->_fullname_field, 0, Qt::AlignCenter);
      layout->addWidget(this->_email_field, 0, Qt::AlignCenter);
      layout->addWidget(this->_password_field, 0,  Qt::AlignCenter);
      layout->addWidget(this->_login_button, 0, Qt::AlignCenter);
      layout->addSpacing(10);
      layout->addWidget(this->_message_field, 0, Qt::AlignCenter);
      layout->addStretch();
      layout->addSpacing(10);
      glayout->addLayout(mlayout);
      {
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(this->_video);
        glayout->addLayout(layout);
      }

      this->setCentralWidget(central_widget);
      connect(this->_login_button, SIGNAL(clicked()),
              this, SLOT(_perform_login_or_register()));
      connect(&this->_state, SIGNAL(login_result(gap_Status)),
              this, SLOT(_login_attempt(gap_Status)));
      connect(this, SIGNAL(logged_in()), &this->_state, SLOT(on_logged_in()));
      connect(this, SIGNAL(registered()), &this->_state, SLOT(on_logged_in()));
      connect(this, SIGNAL(login_failed()), SLOT(show()));
      connect(&this->_state, SIGNAL(register_result(gap_Status)),
              this, SLOT(_register_attempt(gap_Status)));
      connect(&this->_state, SIGNAL(internet_issue(QString const&)),
              this, SLOT(_internet_issue(QString const&)));
      this->update();

      if (this->_email_field->text().isEmpty() && !fist::settings()["Login"].exists("facebook"))
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

      this->_update_geometry();
      this->move(QApplication::desktop()->screen()->rect().center() -
                 this->rect().center());
    }

    void
    Window::_update_geometry()
    {
      this->updateGeometry();
      this->_forgot_password_link->move(
        this->_password_field->width() - this->_forgot_password_link->width() - 5,
        this->_password_field->height() / 2 - this->_forgot_password_link->height() / 2);
      this->_password_field->setTextMargins(12, 0, 5 + this->_forgot_password_link->width(), 0);
      ELLE_ASSERT(this->_video != nullptr);
      // this->_help_link->move(QPoint{this->_video->x() - this->_help_link->width() - 5, 5});
    }

    Window::~Window()
    {
      ELLE_TRACE_SCOPE("%s: destruction", *this);
#ifdef VIDEO
      delete player;
#endif
    }

    void
    Window::mode(fist::login::Mode mode,
                 bool clear_message)
    {
      ELLE_TRACE_SCOPE("Set mode %s", mode);
      this->_mode = mode;
      if (clear_message)
        this->_message_field->clear();
      this->_loading->hide();
      auto show = [this] {
        this->_separator->show();
        this->_facebook_button->show();
        this->_email_field->show();
        this->_password_field->show();
        this->_fullname_field->show();
      };
      switch (mode)
      {
        case Mode::Register:
          show();
          this->_facebook_email_info->hide();
          this->_help_link->setText(
            view::links::help::text);
          view::mode::selected_style(*this->_signup_tabber);
          view::mode::default_style(*this->_login_tabber);
          this->_login_button->setText(view::login_button::register_text);
          this->_fullname_field->show();
          this->_fullname_field->setFocus();
          this->_forgot_password_link->hide();
          break;
        case Mode::Login:
          show();
          this->_facebook_email_info->hide();
          view::mode::selected_style(*this->_login_tabber);
          view::mode::default_style(*this->_signup_tabber);
          this->_login_button->setText(view::login_button::login_text);
          this->_fullname_field->hide();
          this->_email_field->setFocus();
          this->_forgot_password_link->show();
          break;
        case Mode::Loading:
          this->_loading->show();
          break;
        default:
          ;
      }
      this->_update_geometry();
      this->repaint();
    }

    void
    Window::_perform_login_or_register()
    {
      switch (this->mode())
      {
        case Mode::Login:
          this->_login();
          break;
        case Mode::Register:
          this->_register();
          break;
        case Mode::Loading:
        default:
          break;
      }
    }

    bool
    Window::eventFilter(QObject *obj, QEvent *event)
    {
      if (!dynamic_cast<QWidget*>(obj) &&
          !dynamic_cast<QLayout*>(obj))
        return Super::eventFilter(obj, event);

      if (obj == this->_login_tabber)
      {
        if (event->type() == QEvent::MouseButtonRelease &&
            this->_mode == Mode::Register)
          this->mode(Mode::Login);
      }
      else if (obj == this->_signup_tabber)
      {
        if (event->type() == QEvent::MouseButtonRelease &&
            this->_mode == Mode::Login)
          this->mode(Mode::Register);
      }
      else if (obj == this->_video)
      {
        if (event->type() == QEvent::MouseButtonRelease)
        {
#ifdef VIDEO
          if (player->isPaused())
            player->play();
          else
            player->pause();
#else
          auto* movie = static_cast<QLabel*>(this->_video)->movie();
          if (movie->state() == QMovie::MovieState::Paused)
            movie->setPaused(false);
          else if (movie->state() == QMovie::MovieState::Running)
            movie->setPaused(true);
          else
          {
            movie->setPaused(false);
            movie->start();
          }
#endif
        }
      }

      return Super::eventFilter(obj, event);
    }

    void
    Window::_register_attempt(gap_Status status)
    {
      ELLE_TRACE_SCOPE("%s: attempt to register", *this);
      this->mode(Mode::Register);
      elle::SafeFinally unlock_register([&] {
          this->_enable(); this->_password_field->setFocus(); });
      if (status == gap_ok)
      {
        auto email = this->_email_field->text();
        auto password = this->_password_field->text();
        this->clear_credentials(false);
        fist::settings()["Login"].set("email", email);
        this->_save_password(email, password);
        emit registered();
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
    Window::_login_attempt(gap_Status status)
    {
      this->mode(Mode::Login);
      ELLE_TRACE_SCOPE("%s: attempt to login", *this);
      elle::SafeFinally unlock_login([&] {
          this->_facebook_connect_attempt = false;
          this->_enable(); this->_password_field->setFocus(); });
      if (status == gap_ok || status == gap_already_logged_in)
      {
        if (this->_facebook_connect_attempt)
        {
          if (fist::settings()["Login"].exists("facebook"))
            emit logged_in();
          else
            emit registered();
          this->clear_credentials(false);
          fist::settings()["Login"].set("facebook", 1);
        }
        else
        {
          emit logged_in();
          auto email = this->_email_field->text();
          auto password = this->_password_field->text();
          this->clear_credentials(false);
          fist::settings()["Login"].set("email", email);
          this->_save_password(email, password);
        }
        return;
      }
      ELLE_TRACE("login failed");
      emit this->login_failed();
      if (status == gap_deprecated)
      {
        emit version_rejected();
      }
      auto fill_error_field = [&] (std::string const& error_message,
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
        ERR(gap_email_already_registered, "This email has already been taken");
        default:
          fill_error_field(elle::sprintf("Internal error (%s)", status));
      }
#undef ERR
      if (this->_facebook_connect_attempt)
        this->_ask_for_facebook_email();
    }

    void
    Window::clear_credentials(bool clear_cookies)
    {
      ELLE_LOG("%s: clear credentials", *this);
      fist::settings()["Login"].remove("facebook");
      fist::settings()["Login"].remove("password");
      fist::settings()["Login"].remove("email");
      if (clear_cookies)
      {
        std::unique_ptr<facebook::ConnectWindow> f(
          new facebook::ConnectWindow("", nullptr, false));
        f->cookies()->clear();
      }
    }

    void
    Window::_ask_for_facebook_email()
    {
      this->mode(Mode::Register, false);
      this->_facebook_email_info->show();
      this->_facebook_button->hide();
      this->_separator->hide();
      this->_fullname_field->hide();
      this->_password_field->hide();
      this->_update_geometry();
      this->repaint();
    }

    void
    Window::_set_enabled(bool val)
    {
      this->_login_button->setDisabled(!val);
      this->_email_field->setDisabled(!val);
      this->_password_field->setDisabled(!val);
      this->_fullname_field->setDisabled(!val);
      this->_facebook_button->setDisabled(!val);
      this->_login_tabber->setDisabled(!val);
      this->_signup_tabber->setDisabled(!val);
    }

    void
    Window::_enable()
    {
      this->_set_enabled(true);
    }

    void
    Window::_disable()
    {
      this->_set_enabled(false);
    }

    void
    Window::try_auto_login()
    {
      ELLE_TRACE_SCOPE("%s: try auto login", *this);
      if (fist::settings()["Login"].exists("facebook"))
      {
        this->hide();
        this->launch_facebook_connect();
      }
      else if (!this->_password_field->text().isEmpty())
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
    Window::_test_email_field(bool register_)
    {
      if (this->_email_field->text().isEmpty() ||
          !regexp::email::checker.exactMatch(this->_email_field->text()))
      {
        ELLE_DEBUG("invalid email field");
        if (this->_email_field->text().isEmpty())
        {
          this->set_message(
            register_
            ? "You must choose an email address"
            : "You must enter your email address",
            "We need your email, sorry");
        }
        else
          this->set_message("This email address seems invalid",
                            "We need your email, sorry");
        this->_email_field->setDisabled(false);
        this->_email_field->setFocus();
        return false;
      }
      return true;
    }

    bool
    Window::_test_fields(bool register_)
    {
      bool test_fullname = register_;
      if (test_fullname && this->_fullname_field->text().isEmpty())
      {
        ELLE_DEBUG("invalid fullname field");
        this->set_message("You need a name",
                          "It will make you easier to find for your contacts");
        this->_fullname_field->setDisabled(false);
        this->_fullname_field->setFocus();
        return false;
      }
      if (!this->_test_email_field())
        return false;
      if (this->_email_field->text().isEmpty() ||
          !regexp::email::checker.exactMatch(this->_email_field->text()))
      {
        ELLE_DEBUG("invalid email field");
        if (this->_email_field->text().isEmpty())
        {
          this->set_message(
            register_
            ? "You must choose an email address"
            : "You must enter your email address",
            "We need your email, sorry");
        }
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
        this->set_message(
          register_
          ? "Choose a password"
          : "Enter your password",
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
          // this->_footer->mode(Mode::Register);
        });
      this->_disable();
      elle::SafeFinally register_failed_guard([&] { emit this->register_failed(); });
      this->_message_field->clear();
      if (this->_facebook_email_info->isVisible())
      {
        if (!this->_test_email_field())
          return;
        else
          this->fb(this->_facebook_window->token());
        return;
      }

      if (!this->_test_fields(true))
        return;
      auto email_array = this->_email_field->text().toUtf8();
      std::string email(email_array.constData());
      auto pw_array = this->_password_field->text().toUtf8();
      std::string pw(pw_array.constData());
      auto fullname_array = this->_fullname_field->text().toUtf8();
      std::string fullname(fullname_array.constData());
      register_failed_guard.abort();
      ELLE_TRACE("every check passed")
      {
        emit this->register_attempt();
        this->_state.register_(fullname, email, pw);
      }
      unlock_register.abort();
      this->mode(Mode::Loading);
    }

    void
    Window::_login()
    {
      elle::SafeFinally unlock_login([&] {
          this->_enable();
          // this->_footer->mode(Mode::Login);
        });
      this->_disable();
      elle::SafeFinally login_failed_guard([&] { emit this->login_failed(); });
      this->_message_field->clear();
      ELLE_TRACE_SCOPE("%s: login attempt", *this);
      if (!this->_test_fields())
        return;
      auto email_array = this->_email_field->text().toUtf8();
      std::string email(email_array.constData());
      auto pw_array = this->_password_field->text().toUtf8();
      std::string pw(pw_array.constData());
      login_failed_guard.abort();
      ELLE_TRACE("every check passed")
      {
        emit this->login_attempt();
        this->_state.login(email, pw);
      }
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
        view::message::error_style(*this->_message_field);
      else
        view::message::warning_style(*this->_message_field);
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
      // this->_version_field->show();
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
    Window::launch_facebook_connect()
    {
      this->_facebook_window.reset(
        new facebook::ConnectWindow(this->_state.facebook_app_id(), this));
      this->show();
      this->_facebook_window->setWindowModality(Qt::WindowModal);
      connect(this->_facebook_window.get(), SIGNAL(success(QString const&)),
              this, SLOT(fb(QString const&)));
      connect(this->_facebook_window.get(), SIGNAL(failure(QString const&)),
              this, SLOT(facebook_connect_failed(QString const&)));
      this->_facebook_window->show();
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
        emit this->_login_button->click();
    }

    QWidget const*
    Window::active() const
    {
      if (this->_mode == Mode::Register)
        return this->_signup_tabber;
      else if (this->_mode == Mode::Login)
        return this->_login_tabber;
      else if (this->_mode == Mode::Loading)
        return this->_loading;
      elle::unreachable();
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
          !regexp::email::checker.exactMatch(this->_email_field->text()))
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
    Window::paintEvent(QPaintEvent*)
    {
      static int arrow_width = 12;
      static int arrow_height = 6;
      QPainter painter(this);
      // Background.
      {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0xFE, 0xFE, 0xFE));
        auto const& g = this->active()->geometry();
        qreal top_height = this->active()->y() + g.height() + arrow_height + 7;
        qreal arrow_center = this->active()->x() + g.width() / 2;
        qreal right_border = this->_video->geometry().x();
        painter.drawRect(0, 0, this->width(), this->height());
        painter.setBrush(QColor(0xF8, 0xF8, 0xF8));
        painter.setPen(Qt::NoPen);
        QPolygonF lines;
        lines.append({0, top_height});
        lines.append({arrow_center - arrow_width / 2, top_height});
        lines.append({arrow_center, top_height - arrow_height});
        lines.append({arrow_center + arrow_width / 2, top_height});
        lines.append({right_border, top_height});
        QPolygonF p(lines);
        p.append({right_border, 0});
        p.append({(qreal) this->width(), 0});
        p.append({(qreal) this->width(), (qreal) this->height()});
        p.append({0, (qreal) this->height()});
        painter.drawPolygon(p);
        // Arrow.
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QColor(0xE0, 0xE0, 0xE0));
        painter.drawPolyline(lines);
      }
      // Seprator.
      {
        if (this->_separator->isVisible())
        {
          auto left_limit = this->_facebook_button->x();
          auto right_limit =
            this->_facebook_button->x() + this->_facebook_button->width();
          auto y = this->_separator->y() + this->_separator->height() / 2;
          painter.setPen(view::separator::style.color());
          painter.drawLine(left_limit, y, this->_separator->x() - 8, y);
          painter.drawLine(
            this->_separator->x() + this->_separator->width() + 8, y,
            right_limit, y);
        }
      }
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

    void
    Window::fb(QString const& token)
    {
      this->_facebook_connect_attempt = true;
      this->show();
      elle::SafeFinally unlock_login([&] {
          this->_enable(); this->_password_field->setFocus();
          this->_facebook_connect_attempt = false;
        });
      this->_disable();
      {
        auto email = this->_email_field->text();
        emit this->login_attempt();
        this->_state.facebook_connect(token.toStdString(),
                                      email.toStdString());
      }
      this->mode(Mode::Loading);
      unlock_login.abort();
    }

    void
    Window::facebook_connect_failed(QString const& error)
    {
      this->show();
      this->set_message(error, error, true);
    }
  }
}
