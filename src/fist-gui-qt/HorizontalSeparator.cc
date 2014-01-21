#include <fist-gui-qt/HorizontalSeparator.hh>

#include <QPaintEvent>
#include <QPainter>
#include <QBrush>

HorizontalSeparator::HorizontalSeparator(QWidget* parent):
  QFrame(parent)
{
  this->setFrameShape(QFrame::HLine);
  this->setFixedHeight(2);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize
HorizontalSeparator::sizeHint() const
{
  return QSize(0, 2);
}

void
HorizontalSeparator::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setPen(QColor(0xE3, 0xE3, 0xE3));
  painter.drawLine(QPoint(0, 0), QPoint(this->width(), 0));
  painter.setPen(QColor(0xFF, 0xFF, 0xFF));
  painter.drawLine(QPoint(0, 1), QPoint(this->width(), 1));
}
