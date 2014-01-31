#include <fist-gui-qt/IconButton.hh>

IconButton::IconButton(QPixmap const& pixmap,
                       bool shadow,
                       Callback const& cb):
  _has_shadow(shadow),
  _cache(),
  _original(pixmap),
  _icon(),
  _shadow(),
  _callback(cb)
{
  this->_draw_shape(this->_shadow, Qt::black);
  this->_refresh();
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(this, SIGNAL(released()), SLOT(_clicked()));
}

void
IconButton::enable()
{
  if (!this->isEnabled())
  {
    QPushButton::setDisabled(false);
  }
}

void
IconButton::disable()
{
  if (this->isEnabled())
  {
    QPushButton::setDisabled(true);
  }
}

void
IconButton::_draw_shape(QPixmap& pixmap, QColor const& color)
{
  pixmap = QPixmap(this->_original.size());
  pixmap.fill(Qt::transparent); // Triggers the pixmap alpha channel.
  pixmap.fill(color);
  QPainter painter(&pixmap);
  painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  painter.drawPixmap(QPoint(0, 0), this->_original);
}

void
IconButton::_refresh()
{
  QSize size(this->_original.size());
  if (this->hasShadow())
    size += QSize(0, 1);
  this->_cache = QPixmap(size);
  this->_cache.fill(Qt::transparent);
  QPainter painter(&this->_cache);
  if (this->hasShadow())
    painter.drawPixmap(QPoint(0, 1), this->_shadow);
  painter.drawPixmap(QPoint(0, 0), this->_icon);
}

QSize
IconButton::sizeHint() const
{
  return this->_original.size();
}

void
IconButton::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.drawPixmap(QPoint(0, 0), this->_original);
}

void
IconButton::_clicked()
{
  if (this->_callback)
    this->_callback();
}