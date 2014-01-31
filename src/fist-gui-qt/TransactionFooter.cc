#include <fist-gui-qt/TransactionFooter.hh>

TransactionFooter::TransactionFooter():
  Footer(nullptr),
  _menu(new IconButton(QPixmap(":/icons/gear.png"), true)),
  _send(new IconButton(QPixmap(":/icons/transfer.png"), true))
{
  this->_layout->addWidget(this->_menu);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                         QSizePolicy::MinimumExpanding,
                                         QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}

IconButton const&
TransactionFooter::send() const
{
  return *this->_send;
}

IconButton const&
TransactionFooter::menu() const
{
  return *this->_menu;
}