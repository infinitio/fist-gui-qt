#include "TransactionWindow.hh"

TransactionWindow::TransactionWindow(Transaction const& t, QWidget* parent):
  QMainWindow(parent),
  _transaction(t)
{
  this->_user = new QLabel(t.user.name, this);
  this->_file = new QLabel(t.filename, this);
}

TransactionWindow::~TransactionWindow()
{
  delete this->_user;
  delete this->_file;
}

void
TransactionWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    delete this;
}
