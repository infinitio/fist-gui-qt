#include <fist-gui-qt/SendFooter.hh>

#include <iostream>
SendFooter::SendFooter():
  Footer(),
  _back(new IconButton(QPixmap(":/icons/cancel-send.png"))),
  _send(new IconButton(QPixmap(":/icons/transfer.png")))
{
  this->_layout->addWidget(this->_back);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}

IconButton*
SendFooter::back() const
{
  return this->_back;
}

IconButton*
SendFooter::send() const
{
  return this->_send;
}
