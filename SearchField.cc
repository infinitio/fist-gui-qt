#include <QPainter>

#include <fist-gui-qt/SearchField.hh>

static int const margin = 10;

SearchField::SearchField(QWidget* owner):
  QLineEdit(owner)
{
  this->setFrame(false);
  QFont font("Lucida Grande");
  font.setBold(true);
  font.setPixelSize(12);
  this->setFont(font);
  this->setContentsMargins(margin, 0, margin, 0);
  this->setPlaceholderText("Search for a friend ...");
  this->setFixedWidth(320);
  this->setFixedHeight(this->height());

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
SearchField::setIcon(QPixmap const& pixmap)
{
  this->_icon = pixmap.scaled(QSize(20, 20),
                              Qt::KeepAspectRatioByExpanding,
                              Qt::SmoothTransformation);
  int padding = margin;
  if (!this->_icon.isNull())
    padding += margin + this->_icon.width();
  this->setContentsMargins(padding, 0, margin, 0);
  repaint();
}

void
SearchField::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Up)
    emit up_pressed();
  else if (event->key() == Qt::Key_Down)
    emit down_pressed();
  else
    QLineEdit::keyPressEvent(event);
}

QSize
SearchField::sizeHint() const
{
  // XXX: 60 -> width - 'more' button.
  return QSize(320, this->height());
}

void
SearchField::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  // Draw icon
  if (!this->_icon.isNull())
  {
    painter.drawPixmap(QPoint(margin,
                              (this->height() - this->_icon.height()) / 2),
                       this->_icon);
  }

  QLineEdit::paintEvent(event);
}
