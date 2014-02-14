#include <cstdlib>

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
  _value_animation(new QPropertyAnimation(this, "value")),
  _opacity(0.5),
  _opacity_animation(new QPropertyAnimation(this, "opacity")),
  _step(1)
{
  this->_value_animation->setEasingCurve(QEasingCurve::InOutQuad);

  this->_opacity_animation->setEasingCurve(QEasingCurve::InOutQuad);
  this->_opacity_animation->setDuration(1250);
  this->_opacity_animation->setEndValue(0);

  connect(this->_value_animation, SIGNAL(finished()),
          this, SLOT(fade()));

  this->fade();
}

void
SmoothScrollBar::setMaximum(int const& height)
{
  // If the size is different, we need to scroll (up or down) to fit the new
  // size.
  if (this->_maximum != height)
  {
    if (height < this->_maximum)
    {
      this->_maximum = height;
      this->_scroll_to(0, 165);
    }
    else
      this->_maximum = height;
  }

  QPropertyAnimation* animation =
    new QPropertyAnimation(this, "displayMaximum");
  animation->setDuration(200);
  animation->setEndValue(height);
  animation->start();
}

QSize
SmoothScrollBar::sizeHint() const
{
  return QSize(7, 40);
}

void
SmoothScrollBar::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(Qt::black);
  painter.setOpacity(this->_opacity);

  if (this->displayMaximum() == 0)
  {
    return;
  }

  int start = this->height() * this->value() / this->displayMaximum();
  painter.drawRoundedRect(
    QRect(QPoint(0, start),
          QSize(this->width(),
                this->height() * this->displayPageSize() / this->displayMaximum())),
    4, 4);

}

void
SmoothScrollBar::_scroll_to(int value, int speed)
{
  if (value > this->maximum())
    value = this->maximum();
  else if (value < 0)
    value = 0;

  this->_opacity_animation->stop();
  this->setOpacity(0.5);

  if (value != this->_value_target)
  {
    this->_value_animation->stop();
    this->_value_target = value;
    this->_value_animation->setDuration(speed);
    this->_value_animation->setEndValue(value);
    this->_value_animation->start();
  }
  else
  {
    this->fade();
  }

}

void
SmoothScrollBar::_scroll(bool up)
{
  int value = this->_value_target;
  if (up)
  {
    value += this->step();
    value = std::min(value, this->maximum() - this->pageSize());
  }
  else
  {
    value -= this->step();
    value = std::max(value, 0);
  }

  this->_scroll_to(value);
}

void
SmoothScrollBar::wheelEvent(QWheelEvent* event)
{
  this->_scroll(event->delta() < 0);
}

void
SmoothScrollBar::showEvent(QShowEvent*)
{
  this->setOpacity(0.5);
  this->fade();
}

void
SmoothScrollBar::reload()
{
  this->_value_target = 0;
  this->_value_animation->setDuration(600);
  this->_value_animation->setEndValue(0);
  this->_value_animation->start();
}

void
SmoothScrollBar::fade()
{
  this->_opacity_animation->start();
}
