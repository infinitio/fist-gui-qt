#include <fist-gui-qt/SystrayMessage.hh>

namespace fist
{
  Message::Message(QString const& title,
                                 QString const& body,
                                 Icon const& icon,
                                 int duration,
                                 bool always_show)
    : _title(title)
    , _body(body)
    , _icon(icon)
    , _duration(duration)
    , _always_show(always_show)
  {}

  UpdateAvailableMessage::UpdateAvailableMessage(
    QString const& title,
    QString const& body,
    Message::Icon const& icon,
    int duration,
    bool always_show)
    : Message(title, body, icon, duration, always_show)
  {}
}
