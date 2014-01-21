#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent):
  QWidget(parent)
{
  this->setFixedWidth(313);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
ListItem::mousePressEvent(QMouseEvent*)
{
  trigger();
}

void
ListItem::update()
{
}
