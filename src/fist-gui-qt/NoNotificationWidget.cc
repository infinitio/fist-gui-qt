#include <fist-gui-qt/NoNotificationWidget.hh>
#include <fist-gui-qt/globals.hh>

#include <QLabel>

NoNotificationWidget::NoNotificationWidget(QWidget* parent):
  ListItem(parent, Qt::transparent, false),
  _layout(new QHBoxLayout(this))
{
  QLabel* text = new QLabel(QString("No notifications."));
  view::transaction::no_notification::style(*text);
  this->_layout->addWidget(text, 0, Qt::AlignCenter);
}

/*-------.
| Layout |
`-------*/

QSize
NoNotificationWidget::sizeHint() const
{
  return QSize(this->widthHint(), 50);
}

QSize
NoNotificationWidget::minimumSizeHint() const
{
  return this->sizeHint();
}

void
NoNotificationWidget::trigger()
{
}
