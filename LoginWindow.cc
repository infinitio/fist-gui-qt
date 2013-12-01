#include <fist-gui-qt/LoginWindow.hh>

#include <QToolTip>

#include <thread>
#include <iostream>

LoginWindow::LoginWindow(gap_State* state):
  QMainWindow(),
  _state(state)
{
  this->setupUi(this);
}

void
LoginWindow::login()
{
  this->login_button->setDisabled(true);
  std::string email = this->login_field->text().toStdString();
  std::string pw = this->password_field->text().toStdString();
  char* hash = gap_hash_password(_state, email.c_str(), pw.c_str());
  gap_Status status;
  status = gap_login(_state, email.c_str(), hash);
  gap_hash_free(hash);

  if (status == gap_ok)
  {
    auto dock = new InfinitDock(_state);
    dock->show();
    this->deleteLater();
    this->message->setText("");
    return;
  }


  switch (status)
  {
#define ERR(case, msg) \
    case: \
      this->message->setText(tr(msg)); \
      break\
/**/
    ERR(case gap_network_error, "Not connected to internet");
    //: While login from login window
    ERR(case gap_email_password_dont_match, "Wrong login/password");
    ERR(default, "Internal error");
  }
  this->login_button->setDisabled(false);
}

void
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    this->deleteLater();
  else if (event->key() == Qt::Key_Return)
    login();
}
