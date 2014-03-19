#include <fist-gui-qt/TransactionFooter.hh>

TransactionFooter::TransactionFooter():
  Footer(nullptr),
  _back(new IconButton(QPixmap(":/icons/gear.png"))),
  _send(new IconButton(QPixmap(":/icons/transfer.png")))
{
  this->_layout->addWidget(this->_back);
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
TransactionFooter::back() const
{
  return *this->_back;
}
