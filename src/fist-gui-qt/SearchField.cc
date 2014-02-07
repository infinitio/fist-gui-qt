#include <QPainter>

#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/globals.hh>

static int const margin = 7;

SearchField::SearchField(QWidget* owner):
  QLineEdit(owner)
{
  this->setFrame(false);
  view::send::search_field::style(*this);
  this->setContentsMargins(margin, 0, margin, 0);
  this->setPlaceholderText(view::send::search_field::text);
  this->setFixedWidth(320);
  this->setFixedHeight(this->height());
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->setTextMargins(15, 0, 0, 0);
  connect(this, SIGNAL(textChanged(QString const&)),
          SLOT(text_changed(QString const&)));
  this->_search_delay.setSingleShot(true);
  connect(&this->_search_delay, SIGNAL(timeout()),
          this, SLOT(delay_expired()));
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

void
SearchField::text_changed(QString const& text)
{
  if (text.isEmpty())
    emit search_ready(this->text());
  else
  {
    this->_search_delay.start(300);
  }
}

void
SearchField::delay_expired()
{
  emit search_ready(this->text());
}

QSize
SearchField::sizeHint() const
{
  // XXX: 60 -> width - 'more' button.
  return QSize(320, 40);
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
