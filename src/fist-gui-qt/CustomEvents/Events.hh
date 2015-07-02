#ifndef FIST_GUI_QT_CUSTOM_EVENTS_EVENTS_HH
# define FIST_GUI_QT_CUSTOM_EVENTS_EVENTS_HH

# include <QEvent>

namespace fist
{
  const QEvent::Type CONTACT_JOINED = static_cast<QEvent::Type>(QEvent::User + 1);
  const QEvent::Type ACCOUNT_UPDATED = static_cast<QEvent::Type>(QEvent::User + 2);
}

#endif
