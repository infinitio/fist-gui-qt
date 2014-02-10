#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QPainter>
#include <QWheelEvent>

#include <iostream>

#include <fist-gui-qt/ListWidget.hh>

static int const items = 5;

/*-------------.
| Construction |
`-------------*/

ListWidget::ListWidget(QWidget* parent,
                       Separator const& separator):
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
  this->_scroll->hide();
  connect(_scroll, SIGNAL(valueChanged(int)), SLOT(setOffset(int)));
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

/*--------.
| Widgets |
`--------*/

void
ListWidget::add_widget(ListItem* widget, Position position)
{
  if (position == Position::Bottom)
    this->_widgets.push_back(widget);
  else if (position == Position::Top)
    this->_widgets.push_front(widget);

  widget->setParent(this);
  widget->show();
  this->_scroll->raise();
  this->_layout();
}

void
ListWidget::remove_widget(ListItem* widget, bool all)
{
  if (all)
    this->_widgets.removeAll(widget);
  else
  {
    auto index = this->_widgets.indexOf(widget);
    if (index != -1)
      this->_widgets.removeAt(index);
  }
  widget->setParent(nullptr);
  delete widget;
  widget = nullptr;
  this->_layout();
}

void
ListWidget::move_widget(ListItem* widget, Position position)
{
  auto index = this->_widgets.indexOf(widget);
  if (index != -1)
  {
    this->_widgets.removeAt(index);
    this->add_widget(widget, position);
  }
}

QList<ListItem*> const&
ListWidget::widgets() const
{
  return this->_widgets;
}

void
ListWidget::clearWidgets()
{
  for (auto& widget: this->_widgets)
  {
    delete widget;
    widget = nullptr;
  }
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
  int height = 0;
  this->_width_hint = 0;
  for (auto widget: this->_widgets)
    this->_width_hint = std::max(this->_width_hint, widget->sizeHint().width());

  int rows = 0;
  int fixed_height = -1;
  for (auto widget: this->_widgets)
  {
    int widget_height = widget->sizeHint().height();
    QRect geometry(0, height - this->_offset, this->width(), widget_height);
    widget->setGeometry(geometry);
    widget_height = widget->size().height();
    height += widget_height + this->_separator._colors.size();
    if (fixed_height == -1 && ++rows == this->maxRows())
    {
      fixed_height = height;
    }
  }

  this->_width_hint += this->_scroll->sizeHint().width();

  int real_height = ((fixed_height != -1) ? fixed_height : height);

  // Remove the bottom separator.
  if (!this->_widgets.empty())
    real_height -= this->_separator._colors.size();

  this->_height_hint = real_height;
  this->updateGeometry();

  this->_scroll->setMinimum(0);
  this->_scroll->setMaximum(height);
  this->_scroll->setPageSize(this->height());
  this->_scroll->setStep(height / 5);

  if (this->height() < height)
    this->_scroll->show();
  else
    this->_scroll->hide();

  this->_update();
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
ListWidget::keyPressEvent(QKeyEvent*)
{
  return;

  // Problematic with the transactions list ATM.
#if 0
  size_t old_index = _keyboard_index;

  if (event->key() == Qt::Key_Down && _keyboard_index > 0)
    _keyboard_index -= 1;
  else if (event->key() == Qt::Key_Up && _keyboard_index < _widgets.size())
    _keyboard_index += 1;
  else if (event->key() == Qt::Key_Return)
    _widgets[_widgets.size() - _keyboard_index]->trigger();

  std::cout << _widgets.size() << " " << _keyboard_index << std::endl;

  if (_keyboard_index == 0)
  {

    //_mate->setFocus();
  }
  else
  {
    if (old_index > 0)
    {
      ListItem* old = _widgets[_widgets.size() - old_index];
      old->setStyleSheet("background-color:white;");
    }

    ListItem* item = _widgets[_widgets.size() - _keyboard_index];
    item->setStyleSheet("background-color:pink;");
  }
#endif
}

void
ListWidget::setFocus()
{
  QWidget::setFocus();

  if (_widgets.size() != 0)
  {
#if 0
    ListItem* item = _widgets[_widgets.size() - ++_keyboard_index];
    item->setStyleSheet("background-color:pink;");
#endif
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
ListWidget::_update()
{
  for (auto const& widget: this->_widgets)
  {
    widget->update();
  }
  this->update();
}

/*---------.
| Painting |
`---------*/

void
ListWidget::paintEvent(QPaintEvent* e)
{
  QPainter painter(this);
  int height = -this->_offset;

  for (auto widget: this->_widgets)
  {
    height += widget->size().height();
    for (auto const& color: this->_separator._colors)
    {
      painter.setPen(color);
      painter.setBrush(color);
      painter.drawRect(0, height, this->width(), 0);
      height += 1;
    }
  }
}
