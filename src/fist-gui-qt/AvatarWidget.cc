#include <QPainter>
#include <qmath.h>

#include <fist-gui-qt/AvatarWidget.hh>

/*-------------.
| Construction |
`-------------*/

// Diameter of the avatar
static int const picture_size = 50;
static int const shadow_width = 0;
static int const border_width = 0;
static int const progress_size = 4;
static int const progress_offset = -1;
static int const badge_size = 16;
static int const badge_border_size = 0;
static int const total_size = 2 * progress_size + 2 * progress_offset + picture_size;

static int const progress_hue = 200;
static int const progress_saturation = 175;
static int const progress_value = 250;

static QColor const progress_color(
  QColor::fromHsv(progress_hue, progress_saturation, progress_value));

AvatarWidget::AvatarWidget(QWidget* parent)
  : QWidget(parent)
  , _picture(total_size, total_size)
  , _transaction_count(0)
  , _progress(0)
  , _smooth_progress(0)
  , _progress_animation(new QPropertyAnimation(this, "smooth_progress", this))
{
  this->setMinimumSize(total_size, total_size);
  this->setMaximumSize(total_size, total_size);
  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  {
    this->_progress_animation->setDuration(AvatarWidget::progress_update_interval);
    this->_progress_animation->setEasingCurve(QEasingCurve::Linear);
  }
}

AvatarWidget::AvatarWidget(QPixmap const& pixmap,
                           QWidget* parent):
  AvatarWidget(parent)
{
  this->setPicture(pixmap);
}

AvatarWidget::AvatarWidget(QString const& picture, QWidget* parent):
  AvatarWidget(parent)
{
  this->setPicture(picture);
}

/*-------------------.
| Display properties |
`-------------------*/

QPixmap
AvatarWidget::picture()
{
  return this->_picture;
}

void
AvatarWidget::setPicture(QPixmap const& avatar)
{
  {
    this->_picture.fill(Qt::transparent);
    QPainter painter(&this->_picture);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(QPointF(total_size / 2., total_size / 2.));
    // Shadow
    {
      auto const diameter = picture_size + 2 * shadow_width + 2 * border_width;
      QRectF geometry(-diameter / 2., -diameter / 2., diameter, diameter);
      QRadialGradient gradient(QPoint(0, 0), diameter / 2.,
                               QPoint(0, 0), diameter / 2. - border_width);
      gradient.setColorAt(0, QColor(0, 0, 0, 32));
      gradient.setColorAt(1, QColor(0, 0, 0, 0));
      painter.setBrush(gradient);
      painter.setPen(Qt::NoPen);
      painter.drawEllipse(geometry);
    }
    // Border
    {
      auto const diameter = picture_size + 2 * shadow_width;
      QRectF geometry(-diameter / 2., -diameter / 2., diameter, diameter);
      painter.setBrush(Qt::white);
      painter.setPen(Qt::NoPen);
      painter.drawEllipse(geometry);
    }
    // Avatar
    {
      QPixmap mask(picture_size, picture_size);
      {
        mask.fill(Qt::transparent);
        QPainter painter(&mask);
        // painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPointF center(picture_size / 2., picture_size / 2.);
        QRadialGradient gradient(center, picture_size / 2.,
                                 center, picture_size / 2. - 1);
        gradient.setColorAt(1, QColor(255, 255, 255, 0));
        gradient.setColorAt(0, QColor(255, 255, 255, 255));
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRect(QPoint(0, 0), mask.size()));
      }
      QPixmap picture(picture_size, picture_size);
      {
        picture.fill(Qt::transparent);
        QPainter painter(&picture);
        // painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::NoPen);
        if (!avatar.isNull())
        {
          auto v = avatar.scaled(picture.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
          painter.drawPixmap(QPoint(0, 0), v);
        }
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawPixmap(QPoint(0, 0), mask);
      }
      auto const diameter = picture_size;
      QPointF geometry(-diameter / 2., -diameter / 2.);
      painter.setBrush(Qt::NoBrush);
      painter.setPen(Qt::NoPen);
      painter.drawPixmap(geometry, picture);
    }
  }
  Q_EMIT this->onPictureChanged();
  adjustSize();
}

/*-------------.
| Transactions |
`-------------*/

void
AvatarWidget::setTransactionCount(int transactionCount)
{
  this->_transaction_count = transactionCount;
  this->update();
  Q_EMIT onTransactionCountChanged(transactionCount);
}

void
AvatarWidget::setProgress(float progress)
{
  this->_progress_animation->stop();
  this->_progress_animation->setStartValue(this->_progress);
  this->_progress_animation->setEndValue(progress);
  this->_progress_animation->start();
  this->_progress = progress;
  this->update();
  Q_EMIT onProgressChanged(progress);
}

void
AvatarWidget::set_smooth_progress(float progress)
{
  this->_smooth_progress = progress;
  this->update();
}

/*--------.
| Drawing |
`--------*/

static
QRectF
center_rect(int diameter)
{
  return QRectF(-diameter / 2., -diameter / 2., diameter, diameter);
}

void
AvatarWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.translate(QPointF(total_size / 2., total_size / 2.));
  QRectF progress_region(
    center_rect(picture_size + 2 * progress_offset + progress_size));
  QRect outer_region(progress_size,
                     progress_size,
                     2 * shadow_width + picture_size,
                     2 * shadow_width + picture_size);
  QRect picture_region(progress_size + shadow_width,
                       progress_size + shadow_width,
                       picture_size,
                       picture_size);
  QPen border_pen(QColor(220, 220, 220));
  // Background
  {
    painter.drawPixmap(QPointF(-total_size / 2., -total_size / 2.),
                       this->_picture);
  }
  static const int badge_angle = 50;
  // Progress bar
  if (this->_progress > 0)
  {
    QPen pen(progress_color);
    pen.setWidth(progress_size);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawArc(progress_region,
                    badge_angle * 16,
                    -this->_smooth_progress * 360 * 16);
  }
}
