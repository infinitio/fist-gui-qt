#include <QFile>

#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>

ELLE_LOG_COMPONENT("infinit.FIST.IconButton");

IconButton::IconButton(QString const& file,
                       QWidget* parent,
                       QSize const& size,
                       Callback const& cb)
  : QPushButton(parent)
  , _resource(file)
  , _size(size)
  , _original()
  , _callback(cb)
{
  this->set_pixmap(this->_resource, size);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(this, SIGNAL(released()), SLOT(_clicked()));
}

void
IconButton::enterEvent(QEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: enter", *this);
  if (this->isEnabled())
  {
    this->setCursor(QCursor(Qt::PointingHandCursor));
    if (QFile::exists(this->_resource + "-hover"))
      this->set_pixmap(this->_resource + "-hover", this->_size, true);
  }
  QPushButton::enterEvent(event);
}

void
IconButton::leaveEvent(QEvent* event)
{
  ELLE_DEBUG_SCOPE("%s: leave", *this);
  this->set_pixmap(this->_resource, this->_size, true);
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
IconButton::set_pixmap(QString const& resource,
                       QSize const& size,
                       bool hover)
{
  ELLE_DEBUG_SCOPE("%s: set pixmap", *this);
  if (!hover)
    this->_resource = resource;
  this->_original = QPixmap(resource);
  if (size.isValid())
  {
    this->_size = size;
    this->_original = this->_original.scaled(this->_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }
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
  ELLE_DEBUG_SCOPE("%s: clicked", *this);
  if (this->_callback)
    this->_callback();
}
