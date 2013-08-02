#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>

#include "ListWidget.hh"
#include "TransactionList.hh"
#include "TransactionWidget.hh"

TransactionList::TransactionList():
  _list(new ListWidget)
{
  auto layout = new QVBoxLayout(this);
  layout->addWidget(this->_list);
}

TransactionWidget*
TransactionList::addTransaction(QString const& avatar)
{
  auto widget = new TransactionWidget(avatar);
  this->_list->addWidget(widget);
  return widget;
}

bool
TransactionList::event(QEvent* event)
{
  return QWidget::event(event);
}
