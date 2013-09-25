#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>

#include "ListWidget.hh"
#include "TransactionList.hh"
#include "TransactionWidget.hh"

TransactionList::TransactionList():
  _list(new ListWidget(this))
{
  auto layout = new QVBoxLayout(this);
  layout->addWidget(this->_list);
}

TransactionWidget*
TransactionList::addTransaction(Transaction const& t)
{
  auto widget = new TransactionWidget(t);
  this->_list->addWidget(widget);
  return widget;
}

void
TransactionList::setFocus()
{
  this->_list->setFocus();
}

void
TransactionList::keyPressEvent(QKeyEvent*)
{
  this->_list->setFocus();
}

bool
TransactionList::event(QEvent* event)
{
  return QWidget::event(event);
}
