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

#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/LoginFooter.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/Settings.hh>

ELLE_LOG_COMPONENT("infinit.FIST.LoginWindow");

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
LoginWindow::_save_password(QString const& email,
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
LoginWindow::_saved_password(QString const& email) const
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

LoginWindow::LoginWindow(fist::State& state,
                         bool fill_email_and_password_fields):
  RoundShadowWidget(5, 3, Qt::FramelessWindowHint),
  _state(state),
  _email_field(new QLineEdit),
  _password_field(new QLineEdit),
  _message_field(new QLabel),
  _loading_icon(new QMovie(QString(":/icons/loading.gif"), QByteArray(), this)),
  _quit_button(new IconButton(QPixmap(QString(":/icons/onboarding-close.png")))),
  _reset_password_link(new QLabel(view::login::links::forgot_password::text)),
  _create_account_link(new QLabel(view::login::links::need_an_account::text)),
  _version_field(new QLabel),
  _footer(new LoginFooter(this)),
  _login_future(),
  _login_watcher()
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);
  this->setWindowIcon(QIcon(":/images/logo.png"));
  this->resize(view::login::size);
  // Quit button.
  {
    connect(this->_quit_button, SIGNAL(released()),
            this, SIGNAL(quit_request()));
  }
  {
    auto saved_email = fist::settings()["Login"].get("email", "").toString();
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
    logo->setPixmap(QPixmap(QString(":/images/logo-complete.png")));
  }
  // Message field.
  {
    view::login::message::style(*this->_message_field);
  }
  // Loading icon.
  {
  }
  // Create account.
  {
    view::login::links::style(*this->_create_account_link);
    this->_create_account_link->setTextInteractionFlags(
      view::login::links::interration_flags);
    this->_create_account_link->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Maximum);
    this->_create_account_link->setOpenExternalLinks(true);
  }
  // Forgotten password.
  {
    view::login::links::style(*this->_reset_password_link);
    this->_reset_password_link->setTextInteractionFlags(
      view::login::links::interration_flags);
    this->_reset_password_link->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Maximum);
    this->_reset_password_link->setOpenExternalLinks(true);
  }
  // Version.
  {
    view::login::version::style(*this->_version_field);
    this->_version_field->hide();
  }
  // Footer.
  {
    connect(this->_footer, SIGNAL(released()),
            this, SLOT(_login()));
  }
  auto central_widget = new QWidget;
  {
    central_widget->setFixedSize(this->width(), this->height());
  }
  this->_password_field->setEchoMode(QLineEdit::Password);
  auto layout = new QVBoxLayout(central_widget);
  layout->setContentsMargins(0, 2, 0, 0);
  {
    auto hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(7, 0, 7, 0);
    hlayout->addWidget(this->_version_field, 0, Qt::AlignCenter);
    hlayout->addStretch();
    hlayout->addWidget(this->_quit_button, 0, Qt::AlignRight);
    layout->addLayout(hlayout);
  }
  layout->addSpacing(15);
  layout->addWidget(logo, 0, Qt::AlignCenter);
  layout->addStretch();
  layout->addWidget(this->_message_field, 0, Qt::AlignCenter);
  layout->addStretch();
  layout->addWidget(this->_email_field, 0, Qt::AlignCenter);
  layout->addSpacing(5);
  layout->addWidget(this->_password_field, 0,  Qt::AlignCenter);
  layout->addSpacing(5);
  {
    auto hlayout = new QHBoxLayout();

    hlayout->addStretch();
    hlayout->addWidget(this->_create_account_link, 0, Qt::AlignRight);
    hlayout->addSpacing(40);
    hlayout->addWidget(this->_reset_password_link, 0, Qt::AlignLeft);
    hlayout->addStretch();
    layout->addLayout(hlayout, Qt::AlignCenter);
  }
  layout->addStretch();
  layout->addWidget(this->_footer);
  this->setCentralWidget(central_widget);

  connect(&this->_login_watcher, SIGNAL(finished()),
          this, SLOT(_login_attempt()));
  connect(this, SIGNAL(logged_in()), &this->_state, SLOT(on_logged_in()));
  this->update();
}

LoginWindow::~LoginWindow()
{
  ELLE_TRACE_SCOPE("%s: destruction", *this);
}

void
LoginWindow::_login_attempt()
{
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
    case error_code:                            \
      fill_error_field(msg);                    \
      break                                     \
        /**/
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
}

void
LoginWindow::_enable()
{
  this->_footer->setDisabled(false);
  this->_email_field->setDisabled(false);
  this->_password_field->setDisabled(false);
}

void
LoginWindow::_disable()
{
  this->_footer->setDisabled(true);
  this->_email_field->setDisabled(true);
  this->_password_field->setDisabled(true);
}

void
LoginWindow::try_auto_login()
{
  if (!this->_password_field->text().isEmpty())
    this->_login();
}

void
LoginWindow::_login()
{
  elle::SafeFinally unlock_login([&] { this->_enable(); });
  this->_message_field->clear();
  this->_disable();

  ELLE_TRACE_SCOPE("%s: login attempt", *this);

  QString email = this->_email_field->text();
  QString pw = this->_password_field->text();

  if (email.isEmpty() || !email_checker.exactMatch(email))
  {
    ELLE_DEBUG("invalid email field");
    this->_message_field->setText("this email address is invalid");
    this->_email_field->setDisabled(false);
    this->_email_field->setFocus();
    return;
  }
  if (pw.isEmpty())
  {
    ELLE_DEBUG("no password");
    this->_message_field->setText("enter your password");
    this->_password_field->setDisabled(false);
    this->_password_field->setFocus();
    return;
  }

  this->_message_field->setMovie(this->_loading_icon);
  this->_message_field->movie()->start();

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

  this->_message_field->setMovie(this->_loading_icon);
  this->_message_field->movie()->start();

  this->_login_watcher.setFuture(this->_login_future);

  unlock_login.abort();
}

void
LoginWindow::set_message(QString const& message,
                         QString const& tooltip)
{
  this->_message_field->setText(message);
  this->_message_field->setToolTip(tooltip);
}

void
LoginWindow::set_version()
{
  ELLE_TRACE_SCOPE("%s: set version: %s", *this, INFINIT_VERSION);
  this->_version_field->setText(
    QString::fromStdString(
      elle::sprintf("v%s", INFINIT_VERSION)));
  this->_version_field->show();
  this->update();
}

void
LoginWindow::update_available(bool mandatory,
                              QString const& changelog)
{
  ELLE_WARN("update available");
  if (!mandatory)
    this->try_auto_login();
}

void
LoginWindow::download_progress(qint64 downloaded, qint64 total_size)
{
  ELLE_DUMP("progress: %s", 100 * downloaded / total_size);
}

void
LoginWindow::download_ready()
{
  ELLE_TRACE_SCOPE("%s: download ready", *this);
  emit update_application();
}

void
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: key pressed (%s)", *this, event->key());

  if (event->key() == Qt::Key_Escape)
    this->_reduce();
  else if (event->key() == Qt::Key_Return)
    emit this->_footer->click();
}

void
LoginWindow::focusInEvent(QFocusEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: gain focus", *this);

  Super::focusInEvent(event);

  if (this->_email_field->text().isEmpty() ||
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
LoginWindow::_reduce()
{
  ELLE_TRACE_SCOPE("%s: reduce", *this);

  this->setWindowState(Qt::WindowMinimized);
}

void
LoginWindow::print(std::ostream& stream) const
{
  stream << "LoginWindow";
}

void
LoginWindow::closeEvent(QCloseEvent* event)
{
  ELLE_TRACE_SCOPE("%s: close", *this);

  Super::closeEvent(event);

  emit quit_request();
}
