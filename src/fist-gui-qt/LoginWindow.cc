#include <QApplication>
#include <QHBoxLayout>
#include <QPixmap>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QToolTip>
#include <QVBoxLayout>

#include <elle/finally.hh>
#include <elle/log.hh>
#include <version.hh>

#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/LoginFooter.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinti.FIST.LoginWindow");

static QRegExp email_checker(regexp::email,
                             Qt::CaseInsensitive);

LoginWindow::LoginWindow(gap_State* state):
  RoundShadowWidget(5, 3, Qt::FramelessWindowHint),
  _state(state),
  _email_field(new QLineEdit),
  _password_field(new QLineEdit),
  _message_field(new QLabel),
  _quit_button(new IconButton(QPixmap(QString(":/icons/onboarding-close.png")))),
  _reset_password_link(new QLabel(view::login::links::forgot_password::text)),
  _create_account_link(new QLabel(view::login::links::need_an_account::text)),
  _version_field(new QLabel)
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);
  this->setWindowIcon(QIcon(":/images/logo.png"));
  this->resize(view::login::size);
  // Quit button.
  {
    connect(this->_quit_button, SIGNAL(released()),
            this, SIGNAL(quit_request()));
  }
  // Email field.
  {
    this->_email_field->setPlaceholderText(view::login::email::placeholder);
    this->_email_field->setFixedSize(view::login::email::size);
    view::login::email::style(*this->_email_field);
    this->_email_field->setTextMargins(12, 0, 12, 0);

    QSettings settings("Infinit.io", "Infinit");
    settings.beginGroup("Login");
    auto saved_email = settings.value("email", "").toString();
    settings.endGroup();

    if (!saved_email.isEmpty())
      this->_email_field->setText(saved_email);
  }
  // Password field.
  {
    this->_password_field->setPlaceholderText(view::login::password::placeholder);
    this->_password_field->setFixedSize(view::login::password::size);
    view::login::password::style(*this->_password_field);
    this->_password_field->setTextMargins(12, 0, 12, 0);
    QSettings settings("Infinit.io", "Infinit");
    settings.beginGroup("Login");
    auto saved_password = settings.value("password", "").toString();
    settings.endGroup();
    if (!saved_password.isEmpty())
      this->_password_field->setText(saved_password);
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
  // Version
  {
    view::login::version::style(*this->_version_field);
    this->_version_field->hide();
  }
  // Footer.
  this->_footer = new LoginFooter;
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

  this->update();
}

LoginWindow::~LoginWindow()
{
  ELLE_TRACE_SCOPE("%s: destruction", *this);
}

void
LoginWindow::_login()
{
  this->_footer->setDisabled(true);

  elle::SafeFinally unlock_login([&] { this->_footer->setDisabled(false); });
  ELLE_TRACE_SCOPE("%s: login attempt", *this);

  QString email = this->_email_field->text();
  QString pw = this->_password_field->text();

  if (email.isEmpty() || !email_checker.exactMatch(email))
  {
    ELLE_DEBUG("invalid email field");
    this->_message_field->setText("invalid email format");
    this->_email_field->setFocus();
    return;
  }

  char* hash = gap_hash_password(
    _state, email.toStdString().c_str(), pw.toStdString().c_str());
  gap_Status status;
  status = gap_login(_state, email.toStdString().c_str(), hash);
  gap_hash_free(hash);

  if (status == gap_ok)
  {
    QSettings settings("Infinit.io", "Infinit");
    settings.beginGroup("Login");
    settings.setValue("email", email);
    settings.endGroup();

    this->_message_field->clear();

    emit logged_in();

    this->close();

    return;
  }
  else if (status == gap_deprecated)
  {
    emit version_rejected();
  }

  switch (status)
  {
#define ERR(case, msg)                                                         \
    case:                                                                      \
      ELLE_WARN("%s", tr(msg));                                                \
      this->_message_field->setText(tr(msg));                           \
      break                                                                    \
/**/
    ERR(case gap_network_error, "Not connected to internet");
    ERR(case gap_meta_unreachable, "Unable to contact main server");
    ERR(case gap_meta_down_with_message, "Main server is down...");
    ERR(case gap_trophonius_unreachable, "Unable to contact notification server");
    ERR(case gap_email_password_dont_match, "Wrong email/password");
    ERR(default, "Internal error");
  }
}

void
LoginWindow::set_message(QString const& message)
{
  this->_message_field->setText(message);
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
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  ELLE_TRACE_SCOPE("%s: key pressed (%s)", *this, event->key());

  if (event->key() == Qt::Key_Escape)
    this->_reduce();
  else if (event->key() == Qt::Key_Return)
    emit this->_footer->click();
}

void
LoginWindow::focusInEvent(QFocusEvent* event)
{
  ELLE_TRACE_SCOPE("%s: gain focus", *this);

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
