#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/globals.hh>

#include <QLabel>

TextListItem::TextListItem(QString const& string,
                           int height,
                           QWidget* parent)
  : Super(parent, Qt::transparent, false)
  , _layout(new QHBoxLayout(this))
  , _height(height)
  , _text(new QLabel(this))
{
  this->_layout->setContentsMargins(42, 0, 32, 0);
  this->_text->setText(string);
  view::transaction::no_notification::style(*this->_text);
  this->_layout->addWidget(this->_text, 1);
  this->show();
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

void
TextListItem::print(std::ostream& out) const
{
  out << "Text(" << this->_text->text().toStdString() << ")";
}
