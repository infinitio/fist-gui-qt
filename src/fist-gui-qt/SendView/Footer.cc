#include <QPixmap>

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
      this->_layout->addWidget(this->_back);
      this->_layout->addStretch();
      this->_layout->addWidget(this->_send);

      this->peer_transaction_mode();
    }

    void
    Footer::peer_transaction_mode()
    {
      this->_send->set_pixmap(QPixmap(":/icons/transfer.png"));
      this->_send->setToolTip("Send");
      this->update();
    }

    void
    Footer::link_mode()
    {
      this->_send->set_pixmap(QPixmap(":/buttons/upload.png"));
      this->_send->setToolTip("Generate a link");
      this->update();
    }
  }
}
