#include <QPixmap>

#include <fist-gui-qt/SendView/Footer.hh>

namespace fist
{
  namespace sendview
  {
    Footer::Footer()
      : ::Footer()
      , _back(new IconButton(QPixmap(":/send/cancel")))
      , _send(new IconButton(QPixmap(":/send/transfer")))
    {
      this->_back->setToolTip("Cancel");
      this->_layout->addWidget(this->_back);
      this->_layout->addStretch();
      this->_layout->addWidget(this->_send);

      this->mode(Mode::p2p);
    }

    void
    Footer::_mode_implementation()
    {
      switch (this->mode())
      {
        case Mode::p2p:
          this->_send->set_pixmap(QPixmap(":/send/transfer"));
          this->_send->setToolTip("Send");
          this->update();
          break;
        case Mode::link:
          this->_send->set_pixmap(QPixmap(":/send/upload"));
          this->_send->setToolTip("Generate a link");
          this->update();
          break;
      }
    }
  }
}
