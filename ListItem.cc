#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent):
  QWidget(parent)
{
  this->setFixedWidth(305);
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
