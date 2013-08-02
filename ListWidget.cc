#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QPainter>
#include <QWheelEvent>

#include "ListWidget.hh"

static int const items = 5;
static int const separator = 1;

/*-------------.
| Construction |
`-------------*/

ListWidget::ListWidget(QWidget* parent):
  Super(parent),
  _height_hint(0),
  _width_hint(0),
  _offset(0),
  _scroll(new SmoothScrollBar(this)),
  _wheel_event(false)
{
  _scroll->hide();
  connect(_scroll, SIGNAL(valueChanged(int)), SLOT(setOffset(int)));
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

/*--------.
| Widgets |
`--------*/

void
ListWidget::addWidget(QWidget* widget)
{
  this->_widgets.push_back(widget);
  widget->setParent(this);
  widget->show();
  this->_layout();
}

void
ListWidget::clearWidgets()
{
  for (auto const& widget: this->_widgets)
    delete widget;
  this->_widgets.clear();
  this->_layout();
}

/*-------.
| Layout |
`-------*/

QSize
ListWidget::sizeHint() const
{
  return QSize(this->_width_hint, this->_height_hint);
}

QSize
ListWidget::minimumSizeHint() const
{
  // FIXME: minimum height when items are present.
  return QSize(this->_width_hint, 0);
}

void
ListWidget::_layout()
{
  int height = 0;
  int y = 0;
  this->_width_hint = 0;
  for (auto widget: this->_widgets)
    this->_width_hint = std::max(this->_width_hint, widget->sizeHint().width());
  for (auto widget: this->_widgets)
  {
    int widget_height = widget->sizeHint().height();
    QRect geometry(0, y - this->_offset, this->width(), widget_height);
    widget->setGeometry(geometry);
    y += widget_height + separator;
    height += widget_height + separator;
  }
  this->_width_hint += 5 + this->_scroll->sizeHint().width();
  this->_height_hint = height;
  this->updateGeometry();
  // QPropertyAnimation* animation = new QPropertyAnimation(this, "heightHint");
  // animation->setDuration(200);
  // animation->setEndValue(height);
  // animation->start();
  if (y > height)
  {
    this->_scroll->show();
    this->_scroll->setMinimum(0);
    this->_scroll->setMaximum(y);
    this->_scroll->setPageSize(height);
    this->_scroll->setStep(height / 5);
  }
  else
    this->_scroll->hide();
}

int
ListWidget::offset()
{
  return _offset;
}

void
ListWidget::setOffset(int val)
{
  this->_offset = val;
  this->_layout();
}

/*-------.
| Events |
`-------*/

void
ListWidget::wheelEvent(QWheelEvent* event)
{
  if (!this->_wheel_event)
  {
    this->_wheel_event = true;
    QCoreApplication::sendEvent(this->_scroll, event);
    this->_wheel_event = false;
  }
}

void
ListWidget::resizeEvent(QResizeEvent*)
{
  {
    QSize size(this->_scroll->sizeHint());
    _scroll->setGeometry(this->width() - size.width(), 0,
                         size.width(), this->height());
  }
  int height = 0;
  for (auto widget: this->_widgets)
  {
    height += widget->sizeHint().height();
  }
}

/*---------.
| Painting |
`---------*/

void
ListWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setPen(QColor(220, 220, 220));
  painter.setBrush(QColor(220, 220, 220));
  int height = -this->_offset;
  for (auto widget: this->_widgets)
  {
    height += widget->sizeHint().height();
    painter.drawRect(0, height, this->width(), separator - 1);
    height += separator;
  }
}
