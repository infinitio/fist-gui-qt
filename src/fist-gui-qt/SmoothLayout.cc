#include <QEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QWidget>
#include <QLayout>

#include <fist-gui-qt/SmoothLayout.hh>
#include <elle/log.hh>

/*-------------.
| Construction |
`-------------*/

ELLE_LOG_COMPONENT("infinit.FIST.SmoothLayout");

SmoothLayout::SmoothLayout(QWidget* owner,
                           int height_animation_duration):
  Super(owner),
  _height_hint(0),
  _width_hint(0),
  _maximum_height(0),
  _maximum_width(0),
  _height_animation(new QPropertyAnimation(this, "heightHint")),
  _width_animation(new QPropertyAnimation(this, "widthHint"))
{
  this->_height_animation->setDuration(height_animation_duration);
  connect(this->_height_animation, SIGNAL(finished()),
          this, SIGNAL(resized()));

  this->_width_animation->setDuration(0);
}

/*-------.
| Layout |
`-------*/

// void
// SmoothLayout::addSpacing(int height)
// {
//   QWidget* empty = new QWidget(this);
//   empty->setFixedHeight(height);
//   empty->setFixedWidth(1);
// }

// void
// SmoothLayout::addStretch()
// {
//   QWidget* empty = new QWidget(this);
//   empty->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//   empty->setFixedWidth(1);
// }

// void
// SmoothLayout::addLayout(QLayout* l)
// {
//   QWidget* empty = new QWidget(this);
//   empty->setLayout(l);
// }

void
SmoothLayout::childEvent(QChildEvent* event)
{
  Super::childEvent(event);
  if (event->added() || event->removed())
    if (
      dynamic_cast<QWidget*>(event->child())
      || dynamic_cast<QLayout*>(event->child())
      )
    {
      auto* object = static_cast<QObject*>(event->child());
      if (event->added())
      {
        ELLE_DEBUG("install event filter to new object: %s", *object)
          object->installEventFilter(this);
      }
      else if (event->removed())
      {
        ELLE_DEBUG("remove event filter from deleted object: %s", *object)
          object->removeEventFilter(this);
      }
      this->_layout();
    }
}

bool
SmoothLayout::eventFilter(QObject *obj, QEvent *event)
{
  if (!dynamic_cast<QWidget*>(obj)
      && !dynamic_cast<QLayout*>(obj)
    )
    return Super::eventFilter(obj, event);

  if (dynamic_cast<QWidget*>(obj))
  {
    auto action = [&] {
      this->redraw();
    };
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
  }
  else if (QLayout* layout = dynamic_cast<QLayout*>(obj))
  {
    layout->activate();
  }
  return Super::eventFilter(obj, event);
}

void
SmoothLayout::redraw()
{
  this->_layout(); this->update(); this->updateGeometry();
}

void
SmoothLayout::resizeEvent(QResizeEvent* event)
{
  auto widgets = this->_child_widgets();

  QSize size(event->size());
  // Compute total children height hint and number of growing children.
  int total_height = 0;
  int growing = 0;
  for (QWidget* widget: widgets)
  {
    if (widget->isHidden()) continue;

    total_height += widget->sizeHint().height();
    if ((widget->sizePolicy().verticalPolicy() & QSizePolicy::GrowFlag) &&
        (widget->sizeHint().height() != widget->size().height()))
      ++growing;
  }

  int y = 0;
  for (QWidget* widget: widgets)
  {
    if (widget->isHidden()) continue;

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

QList<QWidget*>
SmoothLayout::_child_widgets(bool visible_only) const
{
  QList<QWidget*> widgets;
  for (QObject* child: this->children())
    if (QWidget* widget = dynamic_cast<QWidget*>(child))
    {
      if (visible_only ? widget->isVisible() : true)
        widgets.append(widget);
    }
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
    if (widget->isHidden()) continue;
    QSize hint(widget->sizeHint());
    if (hint.height() > 0)
    {
      // ELLE_LOG("%s: height: %s", widget, hint.height());
      height += hint.height();
    }
    width = std::max(width, hint.width());
  }

  if (this->isVisible())
  {
    if (height != this->_height_hint)
    {
      this->_height_animation->stop();

      if (this->_height_animation->duration() == 0)
      {
        this->setHeightHint(height);
        emit resized();
      }
      else
      {
        this->_height_animation->setEndValue(height);
        this->_height_animation->start();
      }
    }
    if (width != this->_width_hint)
    {
      this->_width_animation->setEndValue(width);
      this->_width_animation->start();
    }
  }
  else
  {
    // ELLE_LOG("update height: %s", height)
    this->setHeightHint(height);
    this->setWidthHint(width);
  }
}

void
SmoothLayout::setHeightHint(int value)
{
  if (value != this->_height_hint)
  {
    ELLE_DEBUG("%s: new height hint: %s", *this, value);
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
    ELLE_DEBUG("%s: new width hint: %s", *this, value);
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

/*----------.
| Printable |
`----------*/
void
SmoothLayout::print(std::ostream& stream) const
{
  stream << "SmoothLayout";
}
