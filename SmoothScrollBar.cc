#include <QWheelEvent>
#include <QPainter>

#include <fist-gui-qt/SmoothScrollBar.hh>

SmoothScrollBar::SmoothScrollBar(QWidget* parent):
  QWidget(parent),
  _minimum(0),
  _maximum(1),
  _pageSize(1),
  _displayMinimum(_minimum),
  _displayMaximum(_maximum),
  _displayPageSize(_pageSize),
  _value(0),
  _value_target(_value),
  _step(1)
{}

QSize
SmoothScrollBar::sizeHint() const
{
  return QSize(10, 40);
}

void
SmoothScrollBar::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(Qt::black);
  painter.setOpacity(0.5);

  int start = this->height() * this->value() / this->displayMaximum();
  painter.drawRoundedRect(
    QRect(QPoint(0, start),
          QSize(this->width(),
                this->height() * this->displayPageSize() / this->displayMaximum())),
    4, 4);
}

void
SmoothScrollBar::wheelEvent(QWheelEvent* event)
{
  int value = this->_value_target;
  if (event->delta() < 0)
  {
    value += this->step();
    value = std::min(value, this->maximum() - this->pageSize());
  }
  else
  {
    value -= this->step();
    value = std::max(value, 0);
  }
  this->_value_target = value;
  QPropertyAnimation* animation = new QPropertyAnimation(this, "value");
  animation->setEasingCurve(QEasingCurve::InOutQuad);
  animation->setDuration(300);
  animation->setEndValue(value);
  animation->start();
}
