#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QPainter>
#include <QWheelEvent>

#include <elle/log.hh>

#include <fist-gui-qt/ListWidget.hh>

ELLE_LOG_COMPONENT("infinit.FIST.ListWidget");

static int const items = 5;

/*-------------.
| Construction |
`-------------*/

ListWidget::ListWidget(QWidget* parent,
                       Separator const& separator,
                       boost::optional<QColor> background_color):
  Super(parent),
  _offset(0),
  _separator(separator),
  _height_hint(0),
  _width_hint(0),
  _max_rows(-1),
  _scroll(new SmoothScrollBar(this)),
  _wheel_event(false),
  _keyboard_index(0)
{
  this->setContentsMargins(0, 0, 0, 0);
  ELLE_DEBUG_SCOPE("%s: creation", *this);
  this->_scroll->hide();
  connect(_scroll, SIGNAL(valueChanged(int)), SLOT(setOffset(int)));
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

  if (background_color)
  {
    QPalette palette = this->palette();
    {
      palette.setColor(QPalette::Window, background_color.get());
    }
    this->setPalette(palette);
  }
}

/*--------.
| Widgets |
`--------*/

void
ListWidget::add_widget(ItemPtr widget, Position position)
{
  ELLE_DEBUG_SCOPE("%s: add widget %s on %s",
                   *this,
                   *widget,
                   (position == Position::Top) ? "top" : "bottom");
  if (position == Position::Bottom)
    this->_widgets.push_back(widget);
  else if (position == Position::Top)
    this->_widgets.push_front(widget);
  else
    ELLE_ERR("%s: unknown widget position %s", *this, position);
  widget->setParent(this);
  {
    QPalette palette = widget->palette();
    palette.setColor(QPalette::Window, this->palette().color(QPalette::Window));
    widget->setPalette(palette);
  }
  widget->show();
  this->_scroll->raise();
  this->_layout();
}

void
ListWidget::remove_widget(ItemPtr widget, bool all)
{
  ELLE_DEBUG_SCOPE("%s: remove widget%s %s", *this, all ? "s" : "", *widget);
  if (all)
  {
    this->_widgets.removeAll(widget);
  }
  else
  {
    auto index = this->_widgets.indexOf(widget);
    if (index != -1)
      this->_widgets.removeAt(index);
  }
  this->_layout();
}

bool
ListWidget::eventFilter(QObject *obj, QEvent *event)
{
  if (!dynamic_cast<QWidget*>(obj))
    return Super::eventFilter(obj, event);

  auto action = [&] { this->_layout(); };
  if (event->type() == QEvent::Hide)
    action();
  else if (event->type() == QEvent::Show)
    action();
  else if (event->type() == QEvent::GraphicsSceneResize)
    action();
  else if (event->type() == QEvent::Resize)
    action();
  else if (event->type() == QEvent::LayoutRequest)
    action();
  else if (event->type() == QEvent::Move)
    action();
  else if (event->type() == QEvent::Paint);
  else if (event->type() == QEvent::Leave);
  else if (event->type() == QEvent::Enter);

  return Super::eventFilter(obj, event);
}

void
ListWidget::move_widget(ItemPtr widget, Position position)
{
  ELLE_DEBUG_SCOPE("%s: move widget %s to position %s",
                     *this, widget, position);
  auto index = this->_widgets.indexOf(widget);
  if (index != -1)
  {
    this->_widgets.removeAt(index);
    this->add_widget(widget, position);
  }
  else
  {
    ELLE_WARN("%s: widget %s not in children", *this, widget);
  }
}

QList<ListWidget::ItemPtr> const&
ListWidget::widgets() const
{
  return this->_widgets;
}

void
ListWidget::clearWidgets()
{
  this->_widgets.clear();
  this->_layout();
}

/*-------.
| Layout |
`-------*/

QSize
ListWidget::sizeHint() const
{
  return QSize(this->_width_hint, this->_height_hint);
}

QSize
ListWidget::minimumSizeHint() const
{
  // FIXME: minimum height when items are present.
  return QSize(this->_width_hint, 0);
}

void
ListWidget::_layout()
{
  ELLE_TRACE_SCOPE("%s: layout request", *this);
  int height = 0;
  this->_width_hint = 0;
  for (auto widget: this->_widgets)
    this->_width_hint = std::max(this->_width_hint, widget->sizeHint().width());
  ELLE_DEBUG("width hint: %s", this->_width_hint);
  int rows = 0;
  int fixed_height = this->maxRows() == 0 ? 0 : -1;
  for (auto widget: this->_widgets)
  {
    if (widget == nullptr || widget->isHidden())
      continue;

    ELLE_DEBUG("widget: %s", *widget)
    {
      int widget_height = widget->sizeHint().height();
      ELLE_DEBUG("height hint: %s", widget_height);
      QRect geometry(0, height - this->_offset, this->width(), widget_height);
      widget->setGeometry(geometry);
      widget_height = widget->size().height();
      ELLE_DEBUG("new height: %s", widget_height);
      height += widget_height + this->_separator.height();
      if (fixed_height == -1 && ++rows == this->maxRows())
      {
        fixed_height = height;
        ELLE_DEBUG("fixed height: %s", fixed_height);
      }
      ELLE_DEBUG("list height: %s", height);
    }
  }
  ELLE_DEBUG("calculated height: %s", height);

  this->_width_hint += this->_scroll->sizeHint().width();

  int real_height = ((fixed_height != -1) ? fixed_height : height);
  ELLE_DEBUG("real height: %s", real_height);

  // Remove the bottom separator.
  if (!this->_widgets.empty())
  {
    real_height -= this->_separator.height();
    height -= this->_separator.height();
  }

  ELLE_DEBUG("real height less last separator: %s", real_height);

  this->_height_hint = real_height;
  this->updateGeometry();

  ELLE_DEBUG("height updated: %s", this->height());

  this->_scroll->setMinimum(0);
  this->_scroll->setMaximum(height);
  this->_scroll->setPageSize(this->height());

  if (this->height() < height)
    this->_scroll->show();
  else
    this->_scroll->hide();

  this->repaint();
}

int
ListWidget::offset()
{
  return _offset;
}

void
ListWidget::setOffset(int val)
{
  this->_offset = val;
  this->_layout();
}

int
ListWidget::maxRows()
{
  return this->_max_rows;
}

void
ListWidget::setMaxRows(int val)
{
  this->_max_rows = val;
  this->_layout();
}

/*-------.
| Events |
`-------*/

void
ListWidget::wheelEvent(QWheelEvent* event)
{
  if (!this->_wheel_event && !this->_scroll->isHidden())
  {
    this->_wheel_event = true;
    QCoreApplication::sendEvent(this->_scroll, event);
    this->_wheel_event = false;
  }
}

void
ListWidget::_select_element(size_t index)
{
  size_t old_index = this->_keyboard_index;

  if (old_index > 0)
  {
    auto old = this->_widgets[old_index];
    old->setStyleSheet("background-color:white;");
  }

  index %= this->_widgets.size();
  auto item = this->_widgets[index];
  item->setStyleSheet("background-color:pink;");
}

void
ListWidget::keyPressEvent(QKeyEvent* event)
{
  return;

#if 0
  // Problematic with the transactions list ATM.
  size_t index = _keyboard_index;

  if (event->key() == Qt::Key_Down && _keyboard_index > 0)
    index -= 1;
  else if (event->key() == Qt::Key_Up && _keyboard_index < _widgets.size())
    index += 1;
  else if (event->key() == Qt::Key_Return)
    _widgets[index]->trigger();
  index %= this->_widgets.size();
  this->_select_element(index);
#endif
}

void
ListWidget::setFocus()
{
  return;

  QWidget::setFocus();

  if (_widgets.size() != 0)
  {
    auto item = _widgets[_widgets.size() - ++_keyboard_index];
    item->setStyleSheet("background-color:pink;");
  }
}

void
ListWidget::resizeEvent(QResizeEvent*)
{
  {
    QSize size(this->_scroll->sizeHint());
    _scroll->setGeometry(this->width() - size.width(), 0,
                         size.width(), this->height());
  }
  this->_layout();
}

void
ListWidget::reload()
{
  for (auto const& widget: this->_widgets)
  {
    widget->reload();
  }

  this->update();
}

/*---------.
| Painting |
`---------*/

void
ListWidget::paintEvent(QPaintEvent* e)
{
  Super::paintEvent(e);
  QPainter painter(this);
  int height = -this->_offset;

  auto left_margin = this->_separator._left_margin;
  auto right_margin = this->_separator._right_margin;
  for (auto widget: this->_widgets)
  {
    height += widget->size().height();
    for (auto const& color: this->_separator._colors)
    {
      painter.setPen(color);
      painter.setBrush(color);
      painter.drawRect(
        left_margin, height, this->width() - left_margin - right_margin, 0);
      height += 1;
    }
  }
}

void
ListWidget::print(std::ostream& stream) const
{
  stream << "ListWidget";
}
