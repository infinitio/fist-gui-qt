#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QPainter>

#include <elle/log.hh>

#include <fist-gui-qt/notification/Notification.hh>
#include <fist-gui-qt/notification/ui.hh>
#include <fist-gui-qt/IconButton.hh>

ELLE_LOG_COMPONENT("infinit.FIST.DesktopIcon");

namespace fist
{
  namespace notification
  {
    INotification::INotification(int duration,
                                 QWidget* parent)
      : Super(parent)
      , _duration(duration)
      , _timer(new QTimer(this))
      , _icon(new QLabel(this))
    {
      this->setWindowFlags(
        Qt::Window | // Add if popup doesn't show up
        Qt::FramelessWindowHint | // No window border
        Qt::WindowStaysOnTopHint // Always on top
        );
      this->setFocusPolicy(Qt::NoFocus);
      connect(this->_timer, SIGNAL(timeout()), this, SLOT(hide()));
      this->_timer->setSingleShot(true);
      this->_timer->start(this->_duration);
      {
        QPalette palette = this->palette();
        {
          palette.setColor(QPalette::Window, QColor{0xFA, 0xFA, 0xFA});
        }
        this->setPalette(palette);
      }
      this->setFixedWidth(360);
      connect(this, SIGNAL(clicked()), SLOT(hide()));
    }

    void
    INotification::enterEvent(QEvent* e)
    {
      ELLE_TRACE_SCOPE("%s: enter", *this);
      this->_timer->stop();
    }

    void
    INotification::leaveEvent(QEvent* e)
    {
      ELLE_TRACE_SCOPE("%s: leave", *this);
      this->_timer->start(1000);
    }

    void
    INotification::mouseReleaseEvent(QMouseEvent* event)
    {
      emit clicked();
    }

    void
    INotification::hideEvent(QHideEvent * event)
    {
      Super::hideEvent(event);
      this->deleteLater();
    }

    void
    INotification::paintEvent(QPaintEvent* event)
    {
      if (this->_icon != nullptr)
      {
        QPainter painter(this);
        painter.setPen(view::background);
        painter.setBrush(view::background);
        auto width = (this->_icon->pixmap() != nullptr && !this->_icon->pixmap()->isNull())
          ? this->_icon->pixmap()->width()
          : this->_icon->width();
        width += view::spacing * 2;
        painter.drawRect(0, 0, width - 2, this->height());
        painter.setPen(Qt::NoPen);
        painter.setBrush(view::background.darker(110));
        painter.drawRect(width, 0, 1, this->height());
      }
      Super::paintEvent(event);
    }

    Notification::Notification(QString const& title,
                               QString const& body,
                               int duration,
                               QPixmap const& pixmap,
                               QWidget* parent)
      : Super(duration, parent)
      , _title(title.isEmpty() ? nullptr : new QLabel(title, this))
      , _body(new QLabel(body, this))
    {
      auto* layout = new QHBoxLayout(this);
      layout->setContentsMargins(view::spacing, 15, 15, 15);
      layout->setSpacing(view::spacing);
      {
        this->_icon->setPixmap(
          pixmap.isNull()
          ? QPixmap(":/notification/logo").scaled(
            view::icon::diameter, view::icon::diameter,
            Qt::KeepAspectRatio, Qt::SmoothTransformation)
          : pixmap);
        layout->addWidget(this->_icon, 0, Qt::AlignVCenter | Qt::AlignTop);
      }
      layout->addSpacing(10);
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(10);
        if (this->_title != nullptr)
        {
          view::title::style(*this->_title);
          vlayout->addWidget(this->_title);
        }
        {
          this->_body->setWordWrap(true);
          view::body::style(*this->_body);
          vlayout->addWidget(this->_body);
        }
        vlayout->addStretch();
        layout->addLayout(vlayout, 1);
      }
      {
        auto* quit = new IconButton(QPixmap(":/notification/quit"), false, this);
        connect(quit, SIGNAL(released()), this, SLOT(hide()));
        layout->addWidget(quit, 0, Qt::AlignRight | Qt::AlignTop);
      }
    }
  }
}
