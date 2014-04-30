#include <QPainter>
#include <QHBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.SearchField");

static int const margin = 7;
static QSize const icon_size(20, 20);

SearchField::Field::Field(QWidget* parent)
  : QLineEdit(parent)
{}

SearchField::Field::Field(QString const& text, QWidget* parent)
  : QLineEdit(text, parent)
{}

void
SearchField::Field::keyPressEvent(QKeyEvent* event)
{
  ELLE_DEBUG("key pressed: %s", event->key())
    QLineEdit::keyPressEvent(event);
}

SearchField::SearchField(QWidget* owner):
  QWidget(owner),
  _icon(new QLabel(this)),
  _search_field(new Field(this))
{
  connect(this->_search_field, SIGNAL(returnPressed()),
          this, SIGNAL(return_pressed()));

  this->setContentsMargins(margin, 0, margin, 0);
  this->_icon->hide();
  this->_icon->setFixedSize(icon_size);
  auto* layout = new QHBoxLayout(this);
  layout->addWidget(this->_icon);
  layout->addWidget(this->_search_field);
  this->_search_field->setFrame(false);
  view::send::search_field::style(*this->_search_field);
  this->_search_field->setContentsMargins(margin, 0, margin, 0);
  this->_search_field->setPlaceholderText(view::send::search_field::text);
  this->_search_field->setFixedHeight(this->height());
  this->_search_field->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
  connect(this->_search_field, SIGNAL(textChanged(QString const&)),
          this, SLOT(text_changed(QString const&)));
  this->_search_delay.setSingleShot(true);
  connect(&this->_search_delay, SIGNAL(timeout()),
          this, SLOT(delay_expired()));
}

void
SearchField::set_icon(QPixmap const& pixmap)
{
  this->_icon->show();
  this->_icon->setPixmap(pixmap.scaled(icon_size,
                                       Qt::KeepAspectRatioByExpanding,
                                       Qt::SmoothTransformation));
  this->update();
}

void
SearchField::set_icon(QMovie& movie)
{
  movie.setScaledSize(icon_size);
  this->_icon->show();
  this->_icon->setMovie(&movie);
  this->_icon->movie()->start();
}

void
SearchField::set_text(QString const& text)
{
  this->_search_field->setText(text);
}

void
SearchField::insert_text(QString const& text)
{
  this->_search_field->insert(text);
  this->_search_field->setFocus();
}

QString
SearchField::text() const
{
  return this->_search_field->text();
}

void
SearchField::clear()
{
  this->_search_field->clear();
}

void
SearchField::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Up)
    emit up_pressed();
  else if (event->key() == Qt::Key_Down)
    emit down_pressed();
  else if (event->key() == Qt::Key_Return)
    emit return_pressed();
}

void
SearchField::text_changed(QString const& text)
{
  if (text.isEmpty())
    emit search_ready(this->_search_field->text());
  else
  {
    this->_search_delay.start(300);
  }
}

void
SearchField::delay_expired()
{
  emit search_ready(this->_search_field->text());
}

void
SearchField::focusInEvent(QFocusEvent* event)
{
  this->_search_field->setFocus();
}

QSize
SearchField::sizeHint() const
{
  // XXX: 60 -> width - 'more' button.
  return QSize(320, 40);
}
