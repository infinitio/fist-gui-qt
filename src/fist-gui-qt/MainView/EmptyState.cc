#include <fist-gui-qt/MainView/EmptyState.hh>
#include <fist-gui-qt/MainView/ui.hh>

#include <QLabel>
#include <QEvent>
#include <QPen>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
# include <QPainterPath>

namespace fist
{
  namespace mainview
  {
    EmptyState::EmptyState(QString const& icon,
                           QString const& body,
                           QString const& text,
                           QWidget* parent,
                           bool show_link)
      : Super(parent, Qt::transparent, false)
      , _layout(new QVBoxLayout(this))
      , _label(new QLabel(text, this))
      , _link(new QLabel("or watch the tutorial again", this))
    {
      this->_layout->setAlignment(Qt::AlignCenter);
      this->_layout->setContentsMargins(25, 50, 25, 10);
      this->_layout->addStretch();
      this->_layout->setSpacing(20);
      {
        auto* logo = new QLabel(this);
        logo->setPixmap(QPixmap(icon));
        this->_layout->addWidget(logo, 1, Qt::AlignCenter);
      }
      {
        auto* label = new QLabel(body, this);
        view::title::style(*label);
        this->_layout->addWidget(label, 1, Qt::AlignCenter);
      }
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(2);
        {
          view::body::style(*this->_label);
          vlayout->addWidget(this->_label, 1, Qt::AlignCenter | Qt::AlignTop);
        }
        {
          if (show_link)
          {
            this->_link->installEventFilter(this);
            view::link::style(*this->_link);
            vlayout->addWidget(this->_link, 1, Qt::AlignCenter | Qt::AlignTop);
          }
          else
          {
            this->_link->hide();
          }
        }
        this->_layout->addLayout(vlayout);
        this->_layout->addSpacing(30);
      }
    }

    void
    EmptyState::paintEvent(QPaintEvent* event)
    {
      QPainter painter(this);
      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
      QPainterPath path;
      QPen pen(Qt::black);
      pen.setWidth(2);
      painter.setPen(pen);
      int x0 = this->_label->x() + this->_label->width() + 10;
      int y0 = this->_label->y() + this->_label->height() / 2;
      int xend = this->width() - 32;
      int yend = this->height() - 20;
      path.moveTo(x0, y0);
      path.cubicTo(this->width() - 30, y0, this->width() - 40, y0 + 20, xend, yend);
      painter.drawPath(path);
      {
        QPainter painter(this);
        painter.setPen(Qt::NoPen); // QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.translate(xend, yend);
        static const QPointF points[3] = {
          QPointF(5, -1),
          QPointF(2, 10),
          QPointF(-5, 1),
        };
        painter.drawPolygon(points, 3);
      }
      Super::paintEvent(event);
    }

    bool
    EmptyState::eventFilter(QObject* obj, QEvent* event)
    {
      if (obj == this->_link)
      {
        if (event->type() == QEvent::Enter)
          this->setCursor(QCursor(Qt::PointingHandCursor));
        else
          this->setCursor(QCursor(Qt::ArrowCursor));
        if (event->type() == QEvent::MouseButtonRelease)
        {
          this->_run_tutorial();
        }
      }
      return Super::eventFilter(obj, event);
    }
  }
}
