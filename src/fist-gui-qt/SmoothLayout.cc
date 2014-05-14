#include <iostream> // FIXME
#include <cassert>

#include <QEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QWidget>

#include <fist-gui-qt/SmoothLayout.hh>
#include <elle/log.hh>

/*-------------.
| Construction |
`-------------*/

ELLE_LOG_COMPONENT("fooo");

SmoothLayout::SmoothLayout(QWidget* owner):
  Super(owner),
  _height_hint(0),
  _width_hint(0),
  _maximum_height(0),
  _maximum_width(0),
  _height_animation(new QPropertyAnimation(this, "heightHint")),
  _width_animation(new QPropertyAnimation(this, "widthHint"))
{
  this->_height_animation->setDuration(100);
  connect(this->_height_animation, SIGNAL(finished()),
          this, SIGNAL(resized()));
  // this->_height_animation->setEasingCurve(QEasingCurve::InOutQuad);

  this->_width_animation->setDuration(0);
}

/*-------.
| Layout |
`-------*/

void
SmoothLayout::childEvent(QChildEvent* event)
{
  Super::childEvent(event);
  if (event->added() || event->removed())
    if (dynamic_cast<QWidget*>(event->child()))
    {
      auto* widget = static_cast<QWidget*>(event->child());
      if (event->added())
      {
        ELLE_WARN("install");

        widget->installEventFilter(this);
      }
      else if (event->removed())
      {
        ELLE_WARN("remove");
        widget->removeEventFilter(this);
      }
      this->_layout();
    }
}

// Filter anchor events to detect changes like visibility changed,
// position changed, modal window spawned...
bool
SmoothLayout::eventFilter(QObject *obj, QEvent *event)
{
  if (!dynamic_cast<QWidget*>(obj))
    return Super::eventFilter(obj, event);

//  auto action = [&] { std::cerr << "event " << event->type() << std::endl; this->_layout(); this->updateGeometry(); };
  auto action = [&] { this->_layout(); this->update(); this->updateGeometry(); }; //std::cerr << "event " << event->type() << std::endl; this->_layout(); this->updateGeometry(); };
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
  // else
  //   std::cerr << "nop: " << event->type() << std::endl;

  return Super::eventFilter(obj, event);
}

void
SmoothLayout::resizeEvent(QResizeEvent* event)
{
  std::cerr << "resize " << event->size() << std::endl;
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
  // res.
  //return QSize(, this->_height_hint);
}

QWidgetList
SmoothLayout::_child_widgets(bool visible_only) const
{
  QWidgetList widgets;
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
  ELLE_LOG("??????");
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

  if (this->isVisible() || true)
  {
    if (height != this->_height_hint)
    {
      this->_height_animation->stop();
      this->_height_animation->setEndValue(height);
      this->_height_animation->start();
      ELLE_LOG("..........................");
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
    // ELLE_LOG("yeap");
    if (this->_maximum_height > 0)
      this->_height_hint = std::min(value, this->_maximum_height);
    else
      this->_height_hint = value;
    Q_EMIT onHeightHintChanged();
    updateGeometry();
  }
  else
  {
    // ELLE_ERR("nop");
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

void
SmoothLayout::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F5)
  {
    std::cerr << "repaint" << std::endl;
    this->repaint(0, 0, 10, 10);
    this->repaint();
  }
  else if (event->key() == Qt::Key_F4)
  {
    std::cerr << "update" << std::endl;
    this->update();
  }
  else if (event->key() == Qt::Key_F3)
  {
    std::cerr << "updage geo" << std::endl;
    this->updateGeometry();
  }
  else if (event->key() == Qt::Key_F2)
  {
    std::cerr << "layout" << std::endl;
    this->resize(0, 0);
    this->_layout();
  }
  else if (event->key() == Qt::Key_F1)
  {
    std::cerr << "resize" << std::endl;
    this->adjustSize();
  }

  Super::keyPressEvent(event);
}
/*----------.
| Printable |
`----------*/
void
SmoothLayout::print(std::ostream& stream) const
{
  stream << "SmoothLayout";
}
