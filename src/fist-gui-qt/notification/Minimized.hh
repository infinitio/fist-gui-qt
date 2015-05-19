#ifndef FIST_GUI_QT_NOTIFICATION_MINIMIZED_HH
# define FIST_GUI_QT_NOTIFICATION_MINIMIZED_HH

# include <fist-gui-qt/notification/Notification.hh>

namespace fist
{
  namespace notification
  {
    class Minimized
      : public INotification
    {
      typedef INotification Super;
    public:
      Minimized(QWidget* parent = nullptr);
    };
  }
}

#endif
