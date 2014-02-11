#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/LoginFooter.hh>
#include <fist-gui-qt/globals.hh>

#include <QToolTip>
#include <QPixmap>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QRegExp>

#include <iostream>

LoginWindow::LoginWindow(gap_State* state):
  RoundShadowWidget(5, 0),
  _state(state),
  _email_field(new QLineEdit),
  _password_field(new QLineEdit),
  _message_field(new QLabel),
  _quit_button(new IconButton(QPixmap(QString(":/icons/onboarding-close.png")))),
  _reset_password_link(new QLabel(view::login::links::forgot_password::text)),
  _create_account_link(new QLabel(view::login::links::need_an_account::text))
{
  {
    QPalette palette = this->palette();
    {
      palette.setColor(QPalette::Window, view::background);
    }
    this->setAutoFillBackground(true);
    this->setPalette(palette);
  }

  this->setWindowIcon(QIcon(":/images/logo.png"));
  this->resize(view::login::size);
  this->setWindowFlags(view::login::flags);

  // Quit button.
  {
    connect(this->_quit_button, SIGNAL(released()),
            this, SLOT(_quit()));
  }

  // Email field.
  {
    this->_email_field->setPlaceholderText(view::login::email::placeholder);
    this->_email_field->setFixedSize(view::login::email::size);
    view::login::email::style(*this->_email_field);
    this->_email_field->setTextMargins(15, 0, 15, 0);
  }

  // Password field.
  {
    this->_password_field->setPlaceholderText(view::login::password::placeholder);
    this->_password_field->setFixedSize(view::login::password::size);
    view::login::password::style(*this->_password_field);
    this->_password_field->setTextMargins(15, 0, 15, 0);
  }

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

  // Footer.
  auto footer = new LoginFooter;
  {
    connect(footer, SIGNAL(released()),
            this, SLOT(_login()));
  }

  auto central_widget = new QWidget;
  {
    central_widget->setFixedSize(this->width(), this->height());
  }

  this->_password_field->setEchoMode(QLineEdit::Password);
  auto layout = new QVBoxLayout(central_widget);
  layout->setContentsMargins(0, 10, 0, 0);
  {
    auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(this->_quit_button, 0, Qt::AlignRight);
    hlayout->addSpacing(10);
    layout->addLayout(hlayout);
  }

  layout->addSpacing(15);
  layout->addWidget(logo, 0, Qt::AlignCenter);
  layout->addStretch();
  layout->addWidget(this->_message_field, 260, Qt::AlignCenter);
  layout->addSpacing(5);
  layout->addWidget(this->_email_field, 260, Qt::AlignCenter);
  layout->addSpacing(5);
  layout->addSpacing(0);
  layout->addWidget(this->_password_field, 260,  Qt::AlignCenter);
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

  layout->addSpacing(20);
  layout->addWidget(footer);

  this->setCentralWidget(central_widget);
  this->update();
}

void
LoginWindow::_login()
{
  // this->_login_button->setDisabled(true);
  QString email = this->_email_field->text();
  QString pw = this->_password_field->text();

  static QRegExp email_checker(regexp::email,
                               Qt::CaseInsensitive);


  if (email.isEmpty() || !email_checker.exactMatch(email))
  {
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
    auto dock = new InfinitDock(_state);
    dock->show();
    this->deleteLater();
    this->_message_field->clear();
    return;
  }

  switch (status)
  {
#define ERR(case, msg)                                                          \
    case:                                                                       \
      this->_message_field->setText(tr(msg));                                   \
      break                                                                     \
/**/
    ERR(case gap_network_error, "Not connected to internet");
    //: While login from login window
    ERR(case gap_email_password_dont_match, "Wrong login/password");
    ERR(default, "Internal error");
  }
  // ythis->_login_button->setDisabled(false);
}

void
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    this->_reduce();
  else if (event->key() == Qt::Key_Return)
    _login();
}

void
LoginWindow::focusInEvent(QFocusEvent*)
{
  this->_email_field->setFocus();
}

void
LoginWindow::_quit()
{
  this->deleteLater();
  this->hide();

  QApplication::setQuitOnLastWindowClosed(true);
  QApplication::quit();
}

void
LoginWindow::_reduce()
{
  this->setWindowState(Qt::WindowMinimized);
}
