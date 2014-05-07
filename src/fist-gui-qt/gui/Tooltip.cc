#include <algorithm>
#include <iostream>

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QShowEvent>

#include <elle/log.hh>

#include <fist-gui-qt/gui/Tooltip.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.gui.Tooltip");

namespace fist
{
  namespace gui
  {
    Tooltip::Tooltip(QString const& text,
                     QWidget* widget,
                     QWidget* parent,
                     Qt::AlignmentFlag aligment,
                     int duration,
                     int margin)
      : QMainWindow(parent, Qt::ToolTip)
      , _text(text)
      , _widget(widget)
      , _margin(margin)
      , _alignment(aligment)
      , _fade_timer(new QTimer(this))
      , _duration_until_fade(duration)
      , _finished(false)
      , _opacity(0.8f)
      , _fade_animation(new QPropertyAnimation(this, "opacity"))
      , _show_animation(new QPropertyAnimation(this, "opacity"))
      , _blocked_by_a_modal_window(0)
    {
      ELLE_TRACE_SCOPE("%s: construction", *this);
      this->setAttribute(Qt::WA_TranslucentBackground, true);

      auto* body = new QWidget(this);
      auto* layout = new QHBoxLayout(body);
      body->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
      layout->setContentsMargins(
        this->_margin * 2, this->_margin, this->_margin * 2, this->_margin);
      {
        auto* label = new QLabel(text, this);
        ::view::tooltip::style(*label);
        label->setAlignment(this->_alignment | Qt::AlignVCenter);
        label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        if (this->_alignment == Qt::AlignLeft)
          layout->addWidget(label, 0, Qt::AlignRight);
        else if (this->_alignment == Qt::AlignRight)
          layout->addWidget(label, 0, Qt::AlignLeft);
      }
      body->adjustSize();
      // Fade timer.
      {
        this->_fade_timer->setSingleShot(true);
        connect(this->_fade_timer, SIGNAL(timeout()),
                this->_fade_animation, SLOT(start()));
      }
      // Animation.show.
      {
        this->_show_animation->setDuration(100);
        this->_show_animation->setEasingCurve(QEasingCurve::Linear);
        this->_show_animation->setStartValue(0.0f);
        this->_show_animation->setEndValue(this->_opacity);
        connect(this, SIGNAL(opacityChanged()),
                this, SLOT(update()));
      }

      // Animation.fade.
      {
        this->_fade_animation->setDuration(600);
        this->_fade_animation->setEasingCurve(QEasingCurve::InOutQuad);
        this->_fade_animation->setStartValue(this->_opacity);
        this->_fade_animation->setEndValue(0.0f);

        connect(this->_fade_animation, SIGNAL(finished()),
                this, SLOT(over()));
        connect(this->_fade_animation, SIGNAL(finished()),
                this, SLOT(hide()));
        connect(this, SIGNAL(opacityChanged()),
                this, SLOT(update()));
      }

      connect(this->_widget, SIGNAL(destroyed()),
              this, SLOT(_anchored_widget_deleted()));

      ELLE_TRACE("install event filter on anchored widget: %s", this->_widget)
        this->_widget->installEventFilter(this);

      if (this->_widget->isVisible())
      {
        ELLE_DEBUG("widget is visible")
          this->show();
      }
      else
      {
        ELLE_DEBUG("widget is hidden")
          this->hide();
      }

      this->setCentralWidget(body);
      this->setFixedSize(this->centralWidget()->size());

      this->update();
    }

    Tooltip::~Tooltip()
    {
      this->over();
    }

    bool
    Tooltip::eventFilter(QObject *obj,
                         QEvent *event)
    {
      ELLE_DEBUG_SCOPE("%s: event filtered for object %s (%s)",
                       *this, obj, event->type());
      if (obj == this->_widget)
      {
        if (event->type() == QEvent::Move)
        {
          if (this->_blocked_by_a_modal_window == 0 &&
              this->_widget->isVisible())
            this->show();
          ELLE_DEBUG("widget moved")
          {
            this->repaint();
          }
        }
        else if (event->type() == QEvent::Paint)
        {
          if (this->_blocked_by_a_modal_window == 0 &&
              this->_widget->isVisible())
            this->show();
          ELLE_DEBUG("widget painted")
          {
            this->repaint();
          }

        }
        else if (event->type() == QEvent::Show)
        {
          ELLE_DEBUG("widget shown")
            this->show();
        }
        else if (event->type() == QEvent::Hide)
        {
          ELLE_DEBUG("widget hidden")
            this->hide();
        }
        else if (event->type() == QEvent::WindowBlocked)
        {
          ELLE_DEBUG("widget blocked by an modal window")
          {
            ++this->_blocked_by_a_modal_window;
            this->hide();
          }
        }
        else if (event->type() == QEvent::WindowActivate)
        {
          ELLE_DEBUG("modal window gone")
          {
            --this->_blocked_by_a_modal_window;
            this->show();
          }
        }
      }
      return QObject::eventFilter(obj, event);
    }

    void
    Tooltip::refresh_position()
    {
      ELLE_DEBUG_SCOPE("refresh position: %s", this->pos());
      if (this->_widget == nullptr)
      {
        this->hide();
      }
      else if (this->_widget->isHidden())
      {
        this->hide();
      }
      else
      {
        this->show();
      }

      if (this->isHidden())
        return;

      auto top_left = this->_widget->mapToGlobal(QPoint(0, 0));
      auto goal = [&] {
        if (this->_alignment == Qt::AlignLeft)
          return top_left - QPoint(
            this->width() + this->_margin,
            -(this->_widget->height() - this->height()) / 2);
        else if (this->_alignment == Qt::AlignRight)
          return top_left + QPoint(
            this->_widget->width() + this->_margin,
            (this->_widget->height() - this->height()) / 2);
        return QPoint(0, 0);
      }();
      if (this->pos() != goal)
      {
        ELLE_DEBUG("move to %s", goal);
        this->move(goal);
        if (this->pos() != goal)
        {
          ELLE_DEBUG("unable to move")
            this->hide();
        }
      }
      else
      {
        ELLE_DEBUG("tooltip is already next to his widget");
      }
    }

    void
    Tooltip::opacity(float opacity)
    {
      this->_opacity = opacity;
      emit this->opacityChanged();
    }

    void
    Tooltip::_anchored_widget_deleted()
    {
      this->_widget = nullptr;
    }

    void
    Tooltip::paintEvent(QPaintEvent*)
    {
      ELLE_DUMP_SCOPE("%s: paint event", *this);
      QPainter painter(this);
      this->refresh_position();
      painter.setRenderHints(QPainter::Antialiasing |
                             QPainter::SmoothPixmapTransform);

      QLinearGradient gradient;
      gradient.setColorAt(0, QColor(0x1D, 0x1D, 0x1D));
      gradient.setColorAt(0.5, QColor(0x4E, 0x4E, 0x4E));
      gradient.setColorAt(1, QColor(0x1D, 0x1D, 0x1D));

      painter.setPen(Qt::NoPen);
      painter.setBrush(gradient);
      painter.setOpacity(this->_opacity);
      QPainterPath path;
      int right = this->width();
      int left = 0;
      int top = 0;
      int bottom = this->height();
      if (this->_alignment == Qt::AlignRight)
        left += 5;
      else if (this->_alignment == Qt::AlignLeft)
        right -= 5;

      int remaining_height = (bottom - top - 10);
      int arrow_height = std::min(int(remaining_height * 0.6), 12);
      int border = (remaining_height - arrow_height) / 2;

      path.moveTo(right, top + 5);
      path.arcTo(right - 10, top, 10, 10, 0.0, 90.0);
      path.lineTo(left + 5, top);
      path.arcTo(left, top, 10, 10, 90.0, 90.0);
      if (this->_alignment == Qt::AlignRight)
      {
        path.lineTo(left, 5 + border);
        path.lineTo(0, this->height() / 2);
        path.lineTo(left, bottom - 5 - border);
      }
      path.lineTo(left, bottom - 5);
      path.arcTo(left, bottom - 10, 10, 10, 180.0, 90.0);
      path.lineTo(right - 5, bottom);
      path.arcTo(right - 10, bottom - 10, 10, 10, 270.0, 90.0);
      if (this->_alignment == Qt::AlignLeft)
      {
        int border = 0.1 * (bottom - top - 10);
        path.lineTo(right, bottom - 5 - border);
        path.lineTo(this->width(), this->height() / 2);
        path.lineTo(right, top + 5 + border);
      }
      path.closeSubpath();
      painter.drawPath(path);
    }

    void
    Tooltip::mouseReleaseEvent(QMouseEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: clicked", *this);
      this->_show_animation->stop();
      this->_fade_animation->start();
      Super::mouseReleaseEvent(event);
    }

    void
    Tooltip::enterEvent(QEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: entered", *this);
      this->_fade_animation->stop();
      this->_fade_timer->stop();
      this->opacity(0.8f);
      Super::enterEvent(event);
    }

    void
    Tooltip::showEvent(QShowEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: shown", *this);
      if (!this->_finished)
      {
        if (this->_fade_animation->state() == QAbstractAnimation::Paused)
          this->_fade_animation->setPaused(false);
        this->_fade_timer->start(this->_duration_until_fade);
        this->_show_animation->stop();
        this->_show_animation->start();
        Super::showEvent(event);
      }
      else
      {
        event->accept();
        this->hide();
      }
    }

    void
    Tooltip::hideEvent(QHideEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: hidden", *this);
      if (this->_fade_timer->isActive())
      {
        this->_fade_timer->stop();
      }
      this->_show_animation->stop();
      if (this->_fade_animation->state() == QAbstractAnimation::Running)
        this->_fade_animation->setPaused(true);
      Super::hideEvent(event);
    }

    void
    Tooltip::leaveEvent(QEvent* event)
    {
      ELLE_DEBUG_SCOPE("%s: leave event", *this);
      this->_show_animation->stop();
      this->_fade_timer->start(std::max(this->_duration_until_fade, 2000));
      Super::leaveEvent(event);
    }

    void
    Tooltip::over()
    {
      this->_finished = true;
      ELLE_TRACE("uninstall event filter on anchored widget: %s", this->_widget)
        if (this->_widget != nullptr)
          this->_widget->removeEventFilter(this);

      this->hide();
      emit hidden();
    }

    /*----------.
    | Printable |
    `----------*/
    void
    Tooltip::print(std::ostream& stream) const
    {
      stream << "Tooltip(";
      if (this->_text.length() < 10)
        stream << this->_text;
      else
        stream << this->_text.left(7).trimmed() << "...";
      stream << ")";
    }

  }
}
