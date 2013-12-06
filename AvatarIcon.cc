#include <fist-gui-qt/AvatarIcon.hh>

# include <QWidget>
# include <QByteArray>
# include <QPixmap>

AvatarIcon::AvatarIcon(QPixmap const& pixmap,
                       QSize const& size):
  _uid(0),
  _pixmap(size),
  _mask(size),
  _geometry(QPoint(0, 0), size)
{
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
