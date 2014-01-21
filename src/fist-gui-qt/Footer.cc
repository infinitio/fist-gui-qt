#include <QPainter>

#include <fist-gui-qt/Footer.hh>

Footer::Footer(QWidget* parent):
  QWidget(parent),
  _layout(new QHBoxLayout(this))
{
  this->_layout->setContentsMargins(10, 10, 10, 5);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  this->setFixedSize(320, 32);
}

void
Footer::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  QLinearGradient gradient;
  gradient.setColorAt(0, QColor(0xF3, 0x63, 0x5F));
  gradient.setColorAt(1, QColor(0xE3, 0x59, 0x55));
  gradient.setStart(QPoint(0, 0));
  gradient.setFinalStop(QPoint(0, this->height()));
  QBrush brush(gradient);
  painter.setBrush(brush);
  painter.setPen(Qt::NoPen);
  painter.drawRect(0, 0, this->width(), this->height());
  painter.setPen(QColor(0xF5, 0x8C, 0x82));
  painter.drawLine(0, 1, this->width(), 1);
}

QSize
Footer::sizeHint() const
{
  return QSize(this->width(), 32);
}

QSize
Footer::minimumSizeHint() const
{
  return this->sizeHint();
}
