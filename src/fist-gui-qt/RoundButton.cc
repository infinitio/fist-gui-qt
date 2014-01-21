#include <fist-gui-qt/RoundButton.hh>

# include <QPen>
# include <QBrush>
# include <QPainter>

# include <iostream>

RoundButton::RoundButton(QColor const& color,
                         QPixmap const& icon,
                         QSize const& size,
                         QWidget* parent):
  QPushButton(parent),
  _color(color),
  _size(size),
  _image(icon.scaled(this->_size.width() * 0.5, this->_size.height() * 0.5)),
  _hover(false)
{
  this->setMaximumSize(this->_size);
}

QSize
RoundButton::sizeHint() const
{
  return this->_size;
}

void
RoundButton::paintEvent(QPaintEvent* /* event */)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QPen pen{Qt::SolidLine};
  pen.setColor(this->_color.lighter(150));
  painter.setPen(pen);

  if (this->_hover)
    painter.setBrush(this->_color.darker(110));
  else
    painter.setBrush(this->_color);

  std::cerr << this->width() << ":" << this->height() << std::endl;
  painter.drawEllipse(0, 0, this->width(), this->height());

  if (!this->_image.isNull())
  {
    int hmid = (this->width() - this->_image.width()) / 2;
    int vmid = (this->height() - this->_image.height()) / 2;
    painter.drawPixmap(QPoint(hmid, vmid + 1), this->_image);
  }
}

void
RoundButton::enterEvent(QEvent*)
{
  this->setCursor(QCursor(Qt::PointingHandCursor));
  this->_hover = true;
}

void
RoundButton::leaveEvent(QEvent*)
{
  this->setCursor(QCursor(Qt::ArrowCursor));
  this->_hover = false;
}
