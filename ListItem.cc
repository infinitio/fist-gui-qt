#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent):
  QWidget(parent)
{
  this->setFixedWidth(305);
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
