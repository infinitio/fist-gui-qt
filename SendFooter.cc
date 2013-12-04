#include <fist-gui-qt/SendFooter.hh>

#include <iostream>
SendFooter::SendFooter():
  Footer(),
  _back(new IconButton(QPixmap(":/icons/trash.png"),
                       true)),
  _send(new IconButton(QPixmap(":/icons/send.png"),
                         true))
{
  this->_layout->addWidget(this->_back);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}

IconButton const*
SendFooter::back() const
{
  return this->_back;
}

IconButton*
SendFooter::send() const
{
  return this->_send;
}
