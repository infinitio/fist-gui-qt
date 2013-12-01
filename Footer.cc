#include <QPainter>

#include <fist-gui-qt/Footer.hh>

Footer::Footer(QWidget* parent):
  QWidget(parent)
{}

void
Footer::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  QLinearGradient gradient;
  gradient.setColorAt(0, QColor(0xEC, 0x3D, 0x2B));
  gradient.setColorAt(1, QColor(0xD2, 0x39, 0x2A));
  gradient.setStart(QPoint(0, 0));
  gradient.setFinalStop(QPoint(0, this->height()));
  QBrush brush(gradient);
  painter.setBrush(brush);
  painter.setPen(Qt::NoPen);
  painter.drawRect(0, 0, this->width(), this->height());
  painter.setPen(QColor(0xF5, 0x8C, 0x82));
  painter.drawLine(0, 1, this->width(), 1);
}
