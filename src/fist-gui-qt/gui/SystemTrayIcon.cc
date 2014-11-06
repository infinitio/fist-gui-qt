#include <fist-gui-qt/gui/SystemTrayIcon.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.FIST.gui.systray.Icon");

namespace fist
{
  namespace gui
  {
    namespace systray
    {
      Icon::Icon(fist::gui::ImageSerie const& serie,
                 QObject* parent)
        : QObject(parent)
        , _icon(new QSystemTrayIcon(this))
        , _rotation_timer(new QTimer(this))
        , _serie(serie)
      {
        this->_rotation_timer->setInterval(80);
        connect(this->_rotation_timer, SIGNAL(timeout()),
                this, SLOT(_change_icon()));
        this->_rotation_timer->start();
      }

      void
      Icon::show()
      {
        this->_icon->setVisible(true);
      }

      void
      Icon::hide()
      {
        this->_icon->setVisible(false);
      }

      void
      Icon::_change_icon()
      {
        this->_icon->setIcon(this->_serie.next());
      }

      void
      Icon::set_icon(QString const& image)
      {
        ELLE_TRACE_SCOPE("%s: set icon %s", *this, image);
        this->_serie = fist::gui::ImageSerie(image);
        this->_rotation_timer->stop();
        this->_change_icon();
        this->show();
      }

      void
      Icon::set_icon(fist::gui::ImageSerie const& serie)
      {
        ELLE_TRACE_SCOPE("%s: set image serie as icon %s", *this, serie);
        this->_serie = serie;
        this->_rotation_timer->start();
        this->show();
      }

      QSystemTrayIcon*
      Icon::inner()
      {
        return this->_icon;
      }
    }
  }
}
