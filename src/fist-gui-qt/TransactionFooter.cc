#include <fist-gui-qt/TransactionFooter.hh>

TransactionFooter::TransactionFooter():
  Footer(nullptr),
  _menu(new IconButton(QPixmap(":/main/gear"))),
  _send(new IconButton(QPixmap(":/main/transfer")))
{
  this->_menu->setToolTip("Options");
  this->_send->setToolTip("Send files");
  this->_layout->addWidget(this->_menu);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                         QSizePolicy::MinimumExpanding,
                                         QSizePolicy::Minimum));
  this->_layout->addWidget(this->_send);
}

void
TransactionFooter::_mode_implementation()
{
  switch (this->mode())
  {
    case fist::Mode::p2p:
      this->_send->set_pixmap(QPixmap(":/send/transfer"));
      this->_send->setToolTip("Send");
      this->update();
      break;
    case fist::Mode::link:
      this->_send->set_pixmap(QPixmap(":/send/upload"));
      this->_send->setToolTip("Generate a link");
      this->update();
      break;
  }
}
