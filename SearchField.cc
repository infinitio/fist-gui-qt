#include <QPainter>

#include "SearchField.hh"

static int const margin = 10;

SearchField::SearchField(QWidget* owner, ListWidget* list):
  QLineEdit(owner),
  _list(list)
{
  this->setFrame(false);
  QFont font("Lucida Grande");
  font.setBold(true);
  font.setPixelSize(12);
  this->setFont(font);
  this->setContentsMargins(margin, 0, margin, 0);
  this->setPlaceholderText("Search for a friend ...");
}

void
SearchField::setIcon(QPixmap const& pixmap)
{
  this->_icon = pixmap;
  int padding = margin;
  if (!this->_icon.isNull())
    padding += margin + this->_icon.width();
  this->setContentsMargins(padding, 0, 0, 0);
  repaint();
}

void
SearchField::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Up)
    _list->setFocus();
  else
    QLineEdit::keyPressEvent(event);
}

QSize
SearchField::sizeHint() const
{
  return QSize(60, 40);
}

void
SearchField::paintEvent(QPaintEvent* event)
{
  QLineEdit::paintEvent(event);

  QPainter painter(this);
  // Draw icon
  if (!this->_icon.isNull())
  {
    painter.drawPixmap(QPoint(margin,
                              (this->height() - this->_icon.height()) / 2),
                       this->_icon);
  }
}
