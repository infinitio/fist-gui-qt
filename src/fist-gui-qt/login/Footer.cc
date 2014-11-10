#include <fist-gui-qt/login/Footer.hh>
#include <fist-gui-qt/globals.hh>

#include <QCursor>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

#include <iostream>

namespace fist
{
  namespace login
  {
    Footer::Footer(QWidget* parent)
      : Super(parent)
      , _loading(new QMovie(QString(":/loading"), QByteArray(), this))
      , _text(new QLabel(this))
      , _mode(Mode::None)
    {
      view::login::footer::style(*this);
      this->_layout->addWidget(this->_text, 0, Qt::AlignCenter);
    }

    void
    Footer::mode(Mode mode)
    {
      this->_mode = mode;
      switch (mode)
      {
        case Mode::Login:
          this->_text->setText(view::login::footer::login_text);
          break;
        case Mode::Register:
          this->_text->setText(view::login::footer::register_text);
          break;
        case Mode::Loading:
          this->_text->setMovie(this->_loading);
          this->_text->movie()->start();
          break;
        default:
          ;
      }
    }

    void
    Footer::loading()
    {
      this->mode(Mode::Loading);
    }

    void
    Footer::mouseReleaseEvent(QMouseEvent*)
    {
      this->click();
    }

    void
    Footer::click()
    {
      if (this->isEnabled())
      {
        switch (this->_mode)
        {
          case Mode::Login:
            emit login();
            break;
          case Mode::Register:
            emit register_();
            break;
          default:
            ;
        }
      }
    }

    void
    Footer::enterEvent(QEvent*)
    {
      if (this->isEnabled())
        this->setCursor(QCursor(Qt::PointingHandCursor));
    }

    void
    Footer::leaveEvent(QEvent*)
    {
      this->setCursor(QCursor(Qt::ArrowCursor));
    }
  }
}
