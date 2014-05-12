#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/TwoStateIconButton.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QDropEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QUrl>

ELLE_LOG_COMPONENT("infinit.FIST.AddFileWidget");

GrowingArea::GrowingArea(QWidget* body,
                         QWidget* parent)
  : QWidget(parent)
  , _body(body)
  , _minimumHeightHint(0)
  , _height_animator(new QPropertyAnimation(this, "minimumHeightHint"))
  , _state(State::shrinked)
{
  this->_height_animator->setDuration(500);
  this->_height_animator->setEasingCurve(QEasingCurve::Linear);

  connect(this->_height_animator, SIGNAL(finished()),
          this, SLOT(_done()));

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(this->_body);
  this->_body->installEventFilter(this);
  this->minimumHeightHint(0);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  // this->setStyleSheet("background-color:blue;");
}

bool
GrowingArea::eventFilter(QObject *obj, QEvent *event)
{
  if (!dynamic_cast<QWidget*>(obj))
    return Super::eventFilter(obj, event);

  auto action = [&] { if (this->_state == State::expanding || this->_state == State::expanded) this->expand(); };
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
  // else if (event->type() == QEvent::Move)
  //   action();
  else if (event->type() == QEvent::Paint);
  else if (event->type() == QEvent::Leave);
  else if (event->type() == QEvent::Enter);
  // else
  //   std::cerr << "nop: " << event->type() << std::endl;

  return Super::eventFilter(obj, event);
}


void
GrowingArea::expand()
{
  this->show();
  std::cerr << this << ": expand" << std::endl;
  if (this->_state == State::expanding || this->_state == State::expanded)
    return;
  this->_state = State::expanding;
  emit expanding();
  this->_height_animator->stop();
  this->_height_animator->setStartValue(this->height());
  this->_height_animator->setEndValue(Super::sizeHint().height());
  std::cerr << "-> " << Super::sizeHint().height() << std::endl;
  this->_height_animator->start();
}

void
GrowingArea::shrink()
{
  this->hide();
  std::cerr << this << ": shrink" << std::endl;
  if (this->_state == State::shrinking || this->_state == State::shrinked)
    return;
  emit shrinking();
  this->_state = State::shrinking;
  this->_height_animator->stop();
  this->_height_animator->setStartValue(this->height());
  this->_height_animator->setEndValue(0);
  this->_height_animator->start();
}

void
GrowingArea::fuuuuuu()
{
  if ((this->_state == State::shrinking) || (this->_state == State::shrinked))
  {
    this->expand();
  }
  else
  {
    this->shrink();
  }
}

void
GrowingArea::_done()
{
  if (this->_state == State::shrinking)
    this->_state = State::shrinked;
  else if (this->_state == State::expanding)
    this->_state = State::expanded;
  std::cerr << "state: " << int(this->_state) << std::endl;
}

AddFileWidget::AddFileWidget(QWidget* body,
                             QWidget* parent):
  QWidget(parent),
  _attach(new IconButton(QPixmap(":/icons/files.png"))),
  _text(new QLabel(view::send::file_adder::text)),
  _expanser(new fist::TwoStateIconButton(QPixmap(":/buttons/show@2x.png"), QPixmap(":/buttons/hide@2x.png"), true)),
  _growing_area(new GrowingArea(body, this)),
  _pulse_animation(new QPropertyAnimation(this, "pulseColor"))
{
  // Background.
  {
    QPalette palette = this->palette();
    {
      palette.setColor(QPalette::Window, view::send::file_adder::background);
    }
    this->setPalette(palette);
    this->setAutoFillBackground(true);
  }

  // Text.
  {
    view::send::file_adder::style(*this->_text);
  }

  // Animation
  {
    this->_pulse_animation->setDuration(600);
    this->_pulse_animation->setEasingCurve(QEasingCurve::InOutQuad);
    this->_pulse_animation->setStartValue(view::send::file_adder::pulse);
    this->_pulse_animation->setEndValue(view::send::file_adder::background);
  }

  auto* vlayout = new QVBoxLayout(this);
  vlayout->setAlignment(Qt::AlignVCenter);
  {
    auto* layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setContentsMargins(16, 10, 6, 10);
    layout->addWidget(this->_attach);
    layout->addWidget(this->_text);
    layout->addStretch();
    layout->addWidget(this->_expanser);
    vlayout->addLayout(layout);
  }
  {
    vlayout->addWidget(this->_growing_area);
  }

  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  // this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->setAcceptDrops(true);

  connect(this->_expanser, SIGNAL(pressed()), this->_growing_area, SLOT(expand()));
  connect(this->_expanser, SIGNAL(released()), this->_growing_area, SLOT(shrink()));
  this->setStyleSheet("background-color:pink;");
};

QSize
AddFileWidget::sizeHint() const
{
  return QSize(320, Super::sizeHint().height()); // QSize(this->width(), 42);
}

// int
// AddFileWidget::widthHint() const
// {
//   return 313;
// }

void
AddFileWidget::pulse()
{
  this->_pulse_animation->stop();
  this->_pulse_animation->start();
}

void
AddFileWidget::setPulseColor(QColor const& color)
{
  QPalette palette;
  palette.setColor(QPalette::Background, color);
  this->setAutoFillBackground(true);
  this->setPalette(palette);
}

void
AddFileWidget::enterEvent(QEvent*)
{
  this->on_entered();
  this->setCursor(QCursor(Qt::PointingHandCursor));
}

void
AddFileWidget::leaveEvent(QEvent*)
{
  this->on_left();
  this->setCursor(QCursor(Qt::ArrowCursor));
}

void
AddFileWidget::mousePressEvent(QMouseEvent*)
{
  emit clicked();
}

void
AddFileWidget::on_entered()
{
  ELLE_DEBUG_SCOPE("%s: mouse entered", *this);
  view::send::file_adder::hover_style(*this->_text);
  this->_attach->set_pixmap(QPixmap(":/icons/files-hover.png"));
  this->repaint();
}

void
AddFileWidget::on_left()
{
  ELLE_DEBUG_SCOPE("%s: mouse left", *this);
  view::send::file_adder::style(*this->_text);
  this->_attach->set_pixmap(QPixmap(":/icons/files.png"));
  this->repaint();
}

void
AddFileWidget::dropEvent(QDropEvent *event)
{
  ELLE_TRACE_SCOPE("%s: drop", *this);

  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        ELLE_DEBUG("%s dropped", url);
        emit file_dropped(url);
      }
  view::send::file_adder::style(*this->_text);
}

void
AddFileWidget::print(std::ostream& stream) const
{
  stream << "File adder";
}
