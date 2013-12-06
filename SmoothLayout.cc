#include <iostream> // FIXME
#include <cassert>

#include <QEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QWidget>

#include <fist-gui-qt/SmoothLayout.hh>

/*-------------.
| Construction |
`-------------*/

SmoothLayout::SmoothLayout(QWidget* owner):
  Super(owner),
  _height_hint(0),
  _width_hint(0),
  _maximum_height(0),
  _maximum_width(0)
{}

/*-------.
| Layout |
`-------*/

void
SmoothLayout::childEvent(QChildEvent* event)
{
  Super::childEvent(event);
  if (event->added() || event->removed())
    if (dynamic_cast<QWidget*>(event->child()))
      this->_layout();
}

void
SmoothLayout::resizeEvent(QResizeEvent* event)
{
  // std::cerr << "resize " << event->size() << std::endl;
  auto widgets = this->_child_widgets();
  QSize size(event->size());
  // Compute total children height hint and number of growing children.
  int total_height = 0;
  int growing = 0;
  for (QWidget* widget: widgets)
  {
    total_height += widget->sizeHint().height();
    if ((widget->sizePolicy().verticalPolicy() & QSizePolicy::GrowFlag) &&
        (widget->sizeHint().height() != widget->size().height()))
      ++growing;
  }

  int y = 0;
  for (QWidget* widget: widgets)
  {
    auto sizeHint = widget->sizeHint();
    auto policy = widget->sizePolicy().horizontalPolicy();
    int width = sizeHint.width();
    int height = sizeHint.height();
    if (growing &&
        (widget->sizePolicy().verticalPolicy() & QSizePolicy::GrowFlag) &&
        (widget->sizeHint().height() != widget->size().height()))
      height += (size.height() - total_height) / growing; // FIXME: rounding
    height = std::max(0, height);
    if (width <= size.width())
    {
      if (policy & QSizePolicy::ExpandFlag)
        width = size.width();
    }
    else
    {
      if (policy & QSizePolicy::ShrinkFlag)
        width = size.width();
    }
    QRect geo(QPoint((size.width() - width) / 2, y), QSize(width, height));
    // std::cerr << "  " << sizeHint << " -> " << geo << std::endl;
    widget->setGeometry(geo);
    y += geo.height();
  }
}

bool
SmoothLayout::event(QEvent* event)
{
  if (event->type() == QEvent::LayoutRequest)
  {
    bool res = Super::event(event);
    this->_layout();
    return res;
  }
  else
    return Super::event(event);
}

QSize
SmoothLayout::sizeHint() const
{
  return QSize(this->_width_hint, this->_height_hint);
}

QWidgetList
SmoothLayout::_child_widgets() const
{
  QWidgetList widgets;
  for (QObject* child: this->children())
    if (QWidget* widget = dynamic_cast<QWidget*>(child))
      widgets.append(widget);
  return widgets;
}

void
SmoothLayout::_layout()
{
  auto widgets = this->_child_widgets();

  int height = 0;
  int width = 0;
  for (QWidget* widget: widgets)
  {
    QSize hint(widget->sizeHint());
    if (hint.height() > 0)
      height += hint.height();
    width = std::max(width, hint.width());
  }

  if (this->isVisible())
  {
    if (height != this->_height_hint)
    {
      QPropertyAnimation* animation =
        new QPropertyAnimation(this, "heightHint");
      animation->setDuration(80);
      animation->setEasingCurve(QEasingCurve::InOutQuad);
      animation->setEndValue(height);
      animation->start();
    }
    if (width != this->_width_hint)
    {
      QPropertyAnimation* animation =
        new QPropertyAnimation(this, "widthHint");
      animation->setDuration(120);
      animation->setEndValue(width);
      animation->start();
    }
  }
  else
  {
    this->setHeightHint(height);
    this->setWidthHint(width);
  }
}

void
SmoothLayout::setHeightHint(int value)
{
  if (value != this->_height_hint)
  {
    if (this->_maximum_height > 0)
      this->_height_hint = std::min(value, this->_maximum_height);
    else
      this->_height_hint = value;
    Q_EMIT onHeightHintChanged();
    updateGeometry();
  }
}

void
SmoothLayout::setWidthHint(int value)
{
  if (value != this->_width_hint)
  {
    if (this->_maximum_width > 0)
      this->_width_hint = std::min(value, this->_maximum_width);
    else
      this->_width_hint = value;
    Q_EMIT onWidthHintChanged();
    updateGeometry();
  }
}

void
SmoothLayout::setMaximumHeight(int value)
{
  if (value != this->_maximum_height)
  {
    this->_maximum_height = value;
    Q_EMIT onMaximumHeightChanged();
    updateGeometry();
  }
}

void
SmoothLayout::setMaximumWidth(int value)
{
  if (value != this->_maximum_width)
  {
    this->_maximum_width = value;
    Q_EMIT onMaximumWidthChanged();
    updateGeometry();
  }
}
