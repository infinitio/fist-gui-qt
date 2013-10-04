#include "LoginWindow.hh"

LoginWindow::LoginWindow(gap_State* state):
  QMainWindow(),
  _msg(nullptr),
  _state(state)
{
  this->resize(180, 130);

  this->_layout = new QVBoxLayout;
  this->_layout->addWidget(this->_login = new QLineEdit(this));
  this->_layout->addWidget(this->_pw = new QLineEdit(this));
  this->_layout->addWidget(this->_button = new QPushButton("Login", this));

  this->_login->setPlaceholderText("E-mail...");
  this->_pw->setPlaceholderText("Password...");

  auto widget = new QWidget;
  widget->setLayout(this->_layout);
  setCentralWidget(widget);

  connect(this->_button, SIGNAL(clicked()), this, SLOT(login()));
}

void
LoginWindow::login()
{
  std::string email("dimrok@infinit.io");
  std::string pw("bitebite");

  if (this->_login->text().toStdString().size() != 0)
  {
    email = std::string(this->_login->text().toStdString());
    pw = std::string(this->_pw->text().toStdString());
  }

  char* hash = gap_hash_password(_state, email.c_str(), pw.c_str());
  gap_Status status = gap_login(_state, email.c_str(), hash);
  gap_hash_free(hash);

  if (status == gap_ok)
  {
    auto dock = new InfinitDock(_state);
    dock->show();

    this->deleteLater();
  }
  else if (_msg == nullptr)
    this->_layout->addWidget(_msg = new QLabel("Wrong username/password."));
}

void
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    this->deleteLater();
  else if (event->key() == Qt::Key_Return)
    login();
}
