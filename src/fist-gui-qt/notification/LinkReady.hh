#ifndef FIST_GUI_QT_NOTIFICATION_LINKREADY_HH
# define FIST_GUI_QT_NOTIFICATION_LINKREADY_HH

# include <fist-gui-qt/notification/Notification.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace notification
  {
    class LinkReady
      : public Notification
    {
      typedef Notification Super;
    public:
      LinkReady(model::Link const& link,
                QWidget* parent = nullptr);

      ELLE_ATTRIBUTE_R(model::Link const&, link);
    };
  }
}

#endif
