#include <elle/log.hh>

#include <fist-gui-qt/DesktopIcon.hh>

ELLE_LOG_COMPONENT("infinit.FIST.DesktopIcon");

DesktopIcon::DesktopIcon(QWidget* parent):
  QMainWindow(parent),
  _expanding(),
  _posx(0),
  _animator(new QPropertyAnimation(this, "posx")),
  _timer(new QTimer(this))
{
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);

  this->_animator->setEasingCurve(QEasingCurve::InOutQuad);
  this->_animator->setDuration(600);

  connect(this->_animator, SIGNAL(finished()),
          this, SLOT(reset_expanding()));

  this->setFixedSize(this->sizeHint());

  connect(this->_timer, SIGNAL(timeout()),
          this, SLOT(collapse()));

  this->_timer->setSingleShot(true);

  this->setStyleSheet("background-color:pink;");

  this->_timer->start(1000);
}

QSize
DesktopIcon::sizeHint() const
{
  return QSize(80, 80);
}

void
DesktopIcon::expand()
{
  ELLE_TRACE_SCOPE("%s: expand", *this);

  int target = 5;
  if (!this->_expanding)
  {
    ELLE_DEBUG("expanding");
    this->_expanding = true;
    this->_animator->stop();
    this->_animator->setEndValue(target);
    this->_animator->start();
  }
}

void
DesktopIcon::collapse()
{
  ELLE_TRACE_SCOPE("%s: collapse", *this);

  int target = 5 - this->sizeHint().width();
  if (this->_expanding)
  {
    ELLE_DEBUG("collapsing");
    this->_expanding = false;
    this->_animator->stop();
    this->_animator->setEndValue(target);
    this->_animator->start();
  }
}

void
DesktopIcon::_reset_expanding()
{
  ELLE_TRACE("reset");
  this->_expanding = boost::logic::tribool::indeterminate_value;
}

void
DesktopIcon::enterEvent(QEvent* e)
{
  ELLE_TRACE_SCOPE("%s: enter", *this);
  this->expand();
}

void
DesktopIcon::leaveEvent(QEvent* e)
{
  ELLE_TRACE_SCOPE("%s: leave", *this);

  if (this->_expanding)
  {
    float progress = this->_animator->currentTime() /
                       (1.0f * this->_animator->totalDuration());

    if (progress < 0.6f)
    {
      this->_reset_expanding();
      this->collapse();
      return;
    }
  }

  this->_timer->start(1800);
}
