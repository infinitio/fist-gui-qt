#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>

#include <QLabel>

TextListItem::TextListItem(QString const& string,
                           int height,
                           QWidget* parent)
  : Super(parent, Qt::transparent, false)
  , _layout(new QHBoxLayout(this))
  , _height(height)
{
  this->_layout->setContentsMargins(42, 0, 32, 0);
  QLabel* text = new QLabel(string);
  view::transaction::no_notification::style(*text);
  this->_layout->addWidget(text, 1);
}

/*-------.
| Layout |
`-------*/

QSize
TextListItem::sizeHint() const
{
  return QSize(Super::sizeHint().width(), this->_height);
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
