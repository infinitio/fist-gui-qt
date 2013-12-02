#include <fist-gui-qt/TransactionFooter.hh>

TransactionFooter::TransactionFooter():
  Footer(nullptr),
  _send(new IconButton(QPixmap(":/icons/arrows.png"),
                       true))
{
  this->_layout->addWidget(new IconButton(QPixmap(":/icons/gear.png"), true));
  this->_layout->addItem(new QSpacerItem(0, 0,
                                         QSizePolicy::MinimumExpanding,
                                         QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}

IconButton* const
TransactionFooter::send() const
{
  return this->_send;
}
