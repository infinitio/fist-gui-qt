#include <fist-gui-qt/SendView/Footer.hh>

namespace fist
{
  namespace sendview
  {
    Footer::Footer():
      ::Footer(),
      _back(new IconButton(QPixmap(":/icons/cancel-send.png"))),
      _send(new IconButton(QPixmap(":/icons/transfer.png")))
    {
      this->_back->setToolTip("Cancel");
      this->_send->setToolTip("Send");
      this->_layout->addWidget(this->_back);
      this->_layout->addItem(new QSpacerItem(0, 0,
                                             QSizePolicy::MinimumExpanding,
                                             QSizePolicy::Minimum));
      this->_layout->addWidget(this->_send);
    }
  }
}
