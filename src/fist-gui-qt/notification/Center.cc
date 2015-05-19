#include <memory>

#include <fist-gui-qt/notification/Center.hh>

#include <QDesktopWidget>

namespace fist
{
  namespace notification
  {
    Manager&
    center()
    {
      static std::unique_ptr<Manager> center;
      // Make it thread safe.
      if (center == nullptr)
        center.reset(new Manager(3, 15));
      return *center;
    }

    Manager::Manager(int max,
                     int margin,
                     QObject* parent)
      : Super(parent)
      , _max(max)
      , _margin(margin)
    {
    }

    INotification*
    Manager::notify(QString const& title,
                      QString const& body,
                      int duration)
    {
      auto* notif = new fist::notification::Notification(
        title,
        body,
        duration);
      this->notify(notif);
      return notif;
    }

    void
    Manager::notify(INotification* notif)
    {
      connect(notif, SIGNAL(destroyed()), this, SLOT(_remove_from_queue()));
      connect(notif, SIGNAL(clicked()), SIGNAL(notification_clicked()));
      this->_notifs.append(notif);
      if (this->_notifs.size() > this->_max)
      {
        this->_notifs.at(0)->hide();
      }
      notif->show();
      this->_reorder();
    }

    void
    Manager::_remove_from_queue()
    {
      auto* notif = static_cast<Notification*>(this->sender());
      this->_notifs.removeAll(notif);
      this->_reorder();
    }

    void
    Manager::_reorder()
    {
      QDesktopWidget f;
      auto geometry = f.availableGeometry();
      auto height = 0;
      for (auto* notif: this->_notifs)
      {
        height += (notif->height() + this->_margin);
        notif->move(geometry.x() + geometry.width() - this->_margin - notif->width(),
                    height);
      }
    }
  }
}
