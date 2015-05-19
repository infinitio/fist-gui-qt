#ifndef FIST_GUI_QT_NOTIFICATION_UPDATEAVAILABLE_HH
# define FIST_GUI_QT_NOTIFICATION_UPDATEAVAILABLE_HH

# include <fist-gui-qt/notification/Notification.hh>

namespace fist
{
  namespace notification
  {
    class UpdateAvailable
      : public Notification
    {
      typedef Notification Super;
    public:
      UpdateAvailable(QWidget* parent = nullptr);
    };
  }
}

#endif
