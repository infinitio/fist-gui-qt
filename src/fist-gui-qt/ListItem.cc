#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent,
                   QColor const& background_color,
                   bool clickable):
  QWidget(parent),
  _background(background_color),
  _clickable(clickable)
{
  this->setFixedWidth(320);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QPalette palette = this->palette();
  {
    palette.setColor(QPalette::Window, this->_background);
  }
  this->setAutoFillBackground(true);
  this->setPalette(palette);
}

QSize
ListItem::sizeHint() const
{
  return QSize(this->widthHint(), this->height());
}

int
ListItem::widthHint() const
{
  // XXX: Fixed width - 7 (width of the scroll bar).
  return 313;
}

void
ListItem::mousePressEvent(QMouseEvent*)
{
  trigger();
}

void
ListItem::enterEvent(QEvent*)
{
  if (!this->_clickable)
    return;

  QPalette palette = this->palette();
  {
    palette.setColor(QPalette::Window, QColor(0xF5, 0xFE, 0xFF));
  }
  this->setPalette(palette);
}

void
ListItem::leaveEvent(QEvent*)
{
  if (!this->_clickable)
    return;

  QPalette palette = this->palette();
  {
    palette.setColor(QPalette::Window, this->_background);
  }
  this->setPalette(palette);
}

void
ListItem::reload()
{
  this->_update();
  this->update();
}

void
ListItem::_update()
{
}
