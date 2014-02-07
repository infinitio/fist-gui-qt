#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent,
                   QColor const& background_color,
                   bool clickable):
  QWidget(parent),
  _background(background_color),
  _clickable(clickable)
{
  this->setFixedWidth(313);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QPalette palette = this->palette();
  {
    palette.setColor(QPalette::Window, this->_background);
  }
  this->setAutoFillBackground(true);
  this->setPalette(palette);
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
ListItem::update()
{
}
