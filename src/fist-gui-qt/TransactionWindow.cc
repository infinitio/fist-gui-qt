#include <fist-gui-qt/TransactionWindow.hh>

# include <iostream>

TransactionWindow::TransactionWindow(QWidget* parent):
  QWidget(parent)
{

}

void
TransactionWindow::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
  {
    parentWidget()->show();
    parentWidget()->setFocus();
    delete this;
  }
}

void
TransactionWindow::mousePressEvent(QMouseEvent*)
{
  this->setFocus();
}
