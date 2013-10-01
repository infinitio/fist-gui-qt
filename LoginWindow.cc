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
  const char* email = "dimrok@infinit.io";
  const char* pw = "bitebite";

  if (this->_login->text().toStdString().size() != 0)
  {
    email = strdup(this->_login->text().toStdString().c_str());
    pw = strdup(this->_pw->text().toStdString().c_str());
  }

  char* hash = gap_hash_password(_state, email, pw);
  gap_Status status = gap_login(_state, email, hash);
  gap_hash_free(hash);

  if (this->_login->text().toStdString().size() != 0)
  {
    delete[] email;
    delete[] pw;
  }

  if (status == gap_ok)
  {
    auto dock = new InfinitDock(_state);
    dock->show();

    // Register gap callbacks.
    gap_connection_callback(_state, InfinitDock::connection_status_cb);

    delete this;
  }
  else if (_msg == nullptr)
    this->_layout->addWidget(_msg = new QLabel("Wrong username/password."));
}

void
LoginWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    delete this;
  else if (event->key() == Qt::Key_Return)
    login();
}
