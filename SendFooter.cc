#include <fist-gui-qt/SendFooter.hh>

#include <iostream>
SendFooter::SendFooter():
  Footer(nullptr),
  _back(new IconButton(QPixmap(":/icons/trash.png"),
                       true)),
  _attach(new IconButton(QPixmap(":/icons/clip.png"),
                         true))

{
  this->_layout->addWidget(this->_back);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Minimum));
  this->_layout->addWidget(this->_attach);
}

IconButton const*
SendFooter::back() const
{
  return this->_back;
}

IconButton const*
SendFooter::attach() const
{
  return this->_attach;
}
