#include <fist-gui-qt/AvatarIcon.hh>

# include <QWidget>
# include <QByteArray>
# include <QPixmap>

AvatarIcon::AvatarIcon(QPixmap const& pixmap,
                       QSize const& size):
  _uid(0),
  _pixmap(size),
  _mask(size),
  _border(1),
  _geometry(QPoint(0, 0), size - QSize(this->_border * 2, this->_border * 2))
{
  this->setMinimumSize(size);
  this->setMaximumSize(size);
  this->set_avatar(pixmap);
}

void
AvatarIcon::set_avatar(QPixmap const& pixmap)
{
  this->_pixmap.fill(Qt::transparent);

  {
    _mask.fill(Qt::transparent);
    QPainter painter(&this->_mask);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(this->_geometry);
  }

  QPainter painter(&this->_pixmap);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.translate(this->_border, this->_border);
  painter.drawPixmap(QPoint(0, 0),
                     pixmap.scaled(this->_geometry.size(),
                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  painter.drawPixmap(this->_geometry, this->_mask);
}

void
AvatarIcon::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawPixmap(_geometry, this->_pixmap);
}
