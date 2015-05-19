#ifndef FIST_GUI_QT_NOTIFICATION_WIDGET_HH
# define FIST_GUI_QT_NOTIFICATION_WIDGET_HH

# include <elle/attribute.hh>

# include <boost/logic/tribool.hpp>
# include <QPixmap>

# include <QTimer>
# include <QWidget>
# include <QLabel>

namespace fist
{
  namespace notification
  {
    class INotification:
      public QWidget
    {
      typedef QWidget Super;
    protected:
      INotification(int duration = 2000,
                    QWidget* parent = nullptr);
    private:
      ELLE_ATTRIBUTE(int, duration);

    private:
      void
      enterEvent(QEvent* e) override;

      void
      leaveEvent(QEvent* e) override;

      void
      mouseReleaseEvent(QMouseEvent* event) override;

      void
      hideEvent(QHideEvent * event) override;

    private:
      QTimer*
      _timer;

    protected:
      QLabel*
      _icon;

    protected:
      void
      paintEvent(QPaintEvent*) override;

    signals:
      void
      clicked();
    private:
      Q_OBJECT;
    };

    class Notification
      : public INotification
    {
      typedef INotification Super;
    public:
      Notification(QString const& title,
                   QString const& body,
                   int duration = 2500,
                   QPixmap const& pixmap = QPixmap(),
                   QWidget* parent = nullptr);

    protected:
      QLabel* _title;
      QLabel* _body;
    };
  }
}

#endif
