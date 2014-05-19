#include <fist-gui-qt/HorizontalSeparator.hh>

#include <QPaintEvent>
#include <QPainter>
#include <QBrush>

HorizontalSeparator::HorizontalSeparator(QWidget* parent,
                                         int margin)
  : QFrame(parent)
  , _margin(margin)
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
  painter.setPen(QColor(0xE5, 0xE5, 0xE5));
  painter.drawLine(QPoint(this->_margin, 0), QPoint(this->width() - this->_margin, 0));
  painter.setPen(Qt::white);
  painter.drawLine(QPoint(this->_margin, 1), QPoint(this->width() , 1));
}
