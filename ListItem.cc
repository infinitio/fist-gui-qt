#include <fist-gui-qt/ListItem.hh>

ListItem::ListItem(QWidget* parent):
  QWidget(parent)
{}

void
ListItem::mousePressEvent(QMouseEvent*)
{
  trigger();
}
