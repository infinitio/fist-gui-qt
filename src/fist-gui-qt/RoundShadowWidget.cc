#include <QEvent>
#include <QPainter>

#include <fist-gui-qt/RoundShadowWidget.hh>
#include <fist-gui-qt/globals.hh>

/*-------------.
| Construction |
`-------------*/

RoundShadowWidget::RoundShadowWidget():
  _radius(5),
  _shadow(10)
{
  int shadow = this->_shadow;
  int margin = this->_radius + shadow;
  this->setContentsMargins(shadow, margin, shadow, margin);
  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setMinimumSize(QSize(0, 0));
  this->setFocusPolicy(Qt::StrongFocus);
}

/*-------------------.
| Display properties |
`-------------------*/

int
RoundShadowWidget::radius()
{
  return this->_radius;
}

void
RoundShadowWidget::setRadius(int value)
{
  this->_radius = value;
  Q_EMIT this->onRadiusChanged();
}

int
RoundShadowWidget::shadow()
{
  return this->_shadow;
}

void
RoundShadowWidget::setShadow(int value)
{
  this->_shadow = value;
  Q_EMIT this->onShadowChanged();
}

/*---------.
| Geometry |
`---------*/

bool
RoundShadowWidget::event(QEvent* event)
{
  if (event->type() == QEvent::LayoutRequest)
  {
    auto res = Super::event(event);
    if (QWidget* widget = this->centralWidget())
    {
      QSize hint(widget->sizeHint());
      hint.setHeight(std::min(500, hint.height()));
      if (this->size() != hint)
        this->resize(hint);
    }
    Q_EMIT onSizeChanged();
    return res;
  }
  else
    return Super::event(event);
}

void
RoundShadowWidget::resizeEvent(QResizeEvent* event)
{
  this->update();
}

/*--------.
| Drawing |
`--------*/

void
RoundShadowWidget::paintEvent(QPaintEvent*)
{
  int const radius = this->_radius;
  int const shadow = this->_shadow;
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  {
    this->_draw_side(painter,
                     0,
                     shadow + radius,
                     shadow,
                     this->height() - (shadow + radius) * 2);
    this->_draw_side(painter,
                     shadow + radius,
                     0,
                     this->width() - (shadow + radius) * 2,
                     shadow);
    this->_draw_side(painter,
                     this->width(),
                     shadow + radius,
                     -shadow,
                     this->height() - (shadow + radius) * 2);
    this->_draw_side(painter,
                     shadow + radius,
                     this->height(),
                     this->width() - (shadow + radius) * 2,
                     -shadow);
  }
  {
    this->_draw_corner(painter,
                       0,
                       0,
                       90);
    this->_draw_corner(painter,
                       this->width() - (shadow + radius) * 2,
                       0,
                       0);
    this->_draw_corner(painter,
                       this->width() - (shadow + radius) * 2,
                       this->height() - (shadow + radius) * 2,
                       270);
    this->_draw_corner(painter,
                       0,
                       this->height() - (shadow + radius) * 2,
                       180);
  }
  painter.setBrush(view::background);
  painter.drawRoundedRect(shadow,
                          shadow,
                          this->width() - shadow * 2,
                          this->height() - shadow * 2,
                          radius,
                          radius);
  // FIXME
  painter.setBrush(QColor(0xE3, 0x59, 0x55));
  painter.drawRoundedRect(shadow,
                          this->height() - shadow - radius * 2,
                          this->width() - shadow * 2,
                          radius * 2,
                          radius,
                          radius);
}

void
RoundShadowWidget::_draw_side(QPainter& painter,
                              int left,
                              int top,
                              int width,
                              int height)
{
  QLinearGradient gradient;
  gradient.setColorAt(0, QColor(0, 0, 0, 0));
  gradient.setColorAt(1, QColor(0, 0, 0, 127));
  gradient.setStart(QPoint(left, top));
  gradient.setFinalStop(QPoint(left + (width < height ? width : 0),
                               top + (height < width ? height : 0)));
  painter.setBrush(QBrush(gradient));
  painter.drawRect(left, top, width, height);
}

void
RoundShadowWidget::_draw_corner(QPainter& painter,
                                int left,
                                int top,
                                int angle)
{
  int const radius = this->_radius;
  int const shadow = this->_shadow;
  QRadialGradient gradient;
  gradient.setRadius(shadow + radius);
  gradient.setColorAt(double(radius) / (radius + shadow), QColor(0, 0, 0, 127));
  gradient.setColorAt(1, QColor(0, 0, 0, 0));
  QPoint center(left + shadow + radius, top + shadow + radius);
  gradient.setCenter(center);
  gradient.setFocalPoint(center);
  painter.setBrush(QBrush(gradient));
  painter.drawPie(QRect(left,
                        top,
                        (shadow + radius) * 2,
                        (shadow + radius) * 2),
                  16 * angle, 16 * 90);
}
