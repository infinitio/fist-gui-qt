#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>

#include <QLabel>

TextListItem::TextListItem(QString const& string,
                           int height,
                           QWidget* parent)
  : ListItem(parent, Qt::transparent, false)
  , _layout(new QHBoxLayout(this))
  , _height(height)
{
  QLabel* text = new QLabel(string);
  view::transaction::no_notification::style(*text);
  this->_layout->addWidget(text, 0, Qt::AlignCenter);
}

/*-------.
| Layout |
`-------*/

QSize
TextListItem::sizeHint() const
{
  return QSize(this->widthHint(), this->_height);
}

QSize
TextListItem::minimumSizeHint() const
{
  return this->sizeHint();
}

void
TextListItem::trigger()
{
}
