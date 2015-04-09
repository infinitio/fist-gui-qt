#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>

ELLE_LOG_COMPONENT("infinit.FIST.IconButton");

IconButton::IconButton(QPixmap const& pixmap,
                       bool shadow,
                       QWidget* parent,
                       Callback const& cb):
  QPushButton(parent),
  _has_shadow(shadow),
  _cache(),
  _original(),
  _icon(),
  _shadow(),
  _callback(cb)
{
  this->set_pixmap(pixmap);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(this, SIGNAL(released()), SLOT(_clicked()));
}

void
IconButton::enterEvent(QEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: enter", *this);

  if (this->isEnabled())
    this->setCursor(QCursor(Qt::PointingHandCursor));
  QPushButton::enterEvent(event);
}

void
IconButton::leaveEvent(QEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: leave", *this);

  this->setCursor(QCursor(Qt::ArrowCursor));
  QPushButton::enterEvent(event);
}

void
IconButton::enable()
{
  ELLE_DEBUG_SCOPE("%s: enable", *this);

  if (!this->isEnabled())
  {
    QPushButton::setDisabled(false);
  }
}

void
IconButton::disable()
{
  ELLE_DEBUG_SCOPE("%s: disabled", *this);

  if (this->isEnabled())
  {
    QPushButton::setDisabled(true);
  }
}

void
IconButton::set_pixmap(QPixmap const& pixmap)
{
  ELLE_DEBUG_SCOPE("%s: set pixmap", *this);
  this->_original = pixmap;
  this->_draw_shape(this->_icon);
  this->_draw_shape(this->_shadow, Qt::black);
  this->_refresh();
  this->update();
}

void
IconButton::_draw_shape(QPixmap& pixmap, QColor const& color)
{
  static QColor empty{};
  pixmap = QPixmap(this->_original.size());
  pixmap.fill(Qt::transparent); // Triggers the pixmap alpha channel.

  QPainter painter(&pixmap);
  if (color != empty)
  {
    pixmap.fill(color);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  }

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
  return this->_cache.size();
}

void
IconButton::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.drawPixmap(QPoint(0, 0), this->_cache);
}

void
IconButton::_clicked()
{
  ELLE_DEBUG_SCOPE("%s: clicked", *this);
  if (this->_callback)
    this->_callback();
}
