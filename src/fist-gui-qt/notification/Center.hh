#ifndef FIST_GUI_QT_NOTIFICATION_MANAGER_HH
# define FIST_GUI_QT_NOTIFICATION_MANAGER_HH

# include <elle/attribute.hh>

# include <QObject>

# include <fist-gui-qt/notification/Notification.hh>

namespace fist
{
  namespace notification
  {
    class Manager
      : public QObject
    {
      typedef QObject Super;
    public:
      Manager(int max = 3,
              int margin = 12,
              QObject* parent = nullptr);

    public slots:
      INotification*
      notify(QString const& title,
               QString const& body,
               int duration = 1500);

      void
      notify(INotification* notify);

    private slots:
      void
      _remove_from_queue();

      void
      _reorder();

    private:
      ELLE_ATTRIBUTE(int, max);
      ELLE_ATTRIBUTE(int, margin);
      ELLE_ATTRIBUTE(QList<INotification*>, notifs);

    signals:
      void
      notification_clicked();
    private:
      Q_OBJECT;
    };

    Manager&
    center();
  }
}


#endif
