#include <fist-gui-qt/AvatarIcon.hh>

# include <QWidget>
# include <QByteArray>
# include <QPixmap>

AvatarIcon::AvatarIcon(QPixmap const& pixmap):
  _uid(0),
  _pixmap(QSize(35, 35)),
  _mask(QSize(35, 35)),
  _geometry(QPoint(0, 0), QSize(35, 35))
{
  QSize size(35, 35);
  this->setMinimumSize(size);
  this->setMaximumSize(size);
  this->_pixmap.fill(Qt::transparent);

  {
    _mask.fill(Qt::transparent);
    QPainter painter(&_mask);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(_geometry);
  }

  QPainter painter(&this->_pixmap);
  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
  painter.drawPixmap(_geometry, pixmap);
  painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  painter.drawPixmap(_geometry, _mask);
}

void
AvatarIcon::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);
  painter.drawPixmap(_geometry, this->_pixmap);
}
