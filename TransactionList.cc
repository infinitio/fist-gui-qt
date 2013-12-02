#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>

#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/TransactionList.hh>
#include <fist-gui-qt/TransactionWidget.hh>

TransactionList::TransactionList():
  _list(new ListWidget(this))
{
  auto layout = new QVBoxLayout(this);
  layout->addWidget(this->_list);
}

TransactionWidget*
TransactionList::addTransaction(gap_State* state, uint32_t tid)
{
  auto widget = new TransactionWidget(state, tid);
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

void
TransactionList::updateTransaction(gap_State* state, uint32_t tid)
{
  // XXX:

  for (auto widget: this->_list->widgets())
    widget->update();
}
