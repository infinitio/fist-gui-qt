#include <QEvent>
#include <QVBoxLayout>

#include <elle/log.hh>
#include <fist-gui-qt/gui/GrowingArea.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.gui.GrowingArea");

namespace fist
{
  namespace gui
  {
    GrowingArea::GrowingArea(QWidget* body,
                             QWidget* parent)
      : QWidget(parent)
      , _body(body)
      , _minimumHeightHint(1)
      , _height_animator(new QPropertyAnimation(this, "minimumHeightHint"))
      , _state(State::expanded)
    {
      this->setContentsMargins(0, 0, 0, 0);
      ELLE_TRACE("%s: creation", *this);
      this->_height_animator->setDuration(200);
      this->_height_animator->setEasingCurve(QEasingCurve::Linear);

      connect(this->_height_animator, SIGNAL(finished()),
              this, SLOT(_done()));

      QVBoxLayout* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(this->_body);
      this->_body->installEventFilter(this);
      this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      this->minimumHeightHint(0);
    }

    bool
    GrowingArea::eventFilter(QObject *obj, QEvent *event)
    {
      if (!dynamic_cast<QWidget*>(obj))
        return Super::eventFilter(obj, event);

      auto action = [&]
      {
        // Ignore everything if the widget is currently shrinking or is already
        // shrinked.
        if (this->_state == State::expanding || this->_state == State::expanded)
          this->expand(true);
      };
      if (event->type() == QEvent::Hide)
        action();
      else if (event->type() == QEvent::Show)
        action();
      else if (event->type() == QEvent::GraphicsSceneResize)
        action();
      else if (event->type() == QEvent::Resize)
      {
        ELLE_DEBUG("element resized");
        action();
      }
      else if (event->type() == QEvent::LayoutRequest)
      {
        ELLE_DEBUG("element layouted");
        action();
      }

      return Super::eventFilter(obj, event);
    }

    void
    GrowingArea::expand(bool force)
    {
      ELLE_DEBUG_SCOPE("%s: expand", *this);
      auto target = this->_body->sizeHint().height(); // Super::sizeHint().height();
      if (this->_height_animator->endValue() == target)
        return;
      if (this->_state == State::expanding)
      {
        ELLE_DEBUG("already expanding");
        return;
      }
      if (!force && this->_state == State::expanded)
      {
        ELLE_DEBUG("already expanded");
        return;
      }
      this->_state = State::expanding;
      ELLE_DEBUG("body height hint: %s", this->_body->sizeHint().height());
      ELLE_DEBUG("super height hint: %s", Super::sizeHint().height());
      ELLE_DEBUG("expand from %s to %s", this->height(), target);
      this->_height_animator->stop();
      this->_height_animator->setStartValue(this->height());
      this->_height_animator->setEndValue(target);
      this->_height_animator->start();
      emit expanding();
    }

    void
    GrowingArea::shrink()
    {
      ELLE_DEBUG_SCOPE("%s: shrink", *this);
      if (this->_state == State::shrinking || this->_state == State::shrinked)
      {
        ELLE_DEBUG("already shrink(ing,ed)");
        return;
      }
      ELLE_DEBUG("shrink from %s to 0", this->height());
      this->_state = State::shrinking;
      this->_height_animator->stop();
      this->_height_animator->setStartValue(this->height());
      this->_height_animator->setEndValue(0);
      this->_height_animator->start();
      emit shrinking();
    }

    void
    GrowingArea::_done()
    {
      ELLE_DEBUG_SCOPE("%s: animation done", *this);
      if (this->_state == State::shrinking)
      {
        ELLE_DEBUG("shrinked");
        this->_state = State::shrinked;
        emit shrinked();
      }
      else if (this->_state == State::expanding)
      {
        ELLE_DEBUG("expanded");
        this->_state = State::expanded;
        emit expanded();
      }
    }

    QSize
    GrowingArea::minimumSizeHint() const
    {
      return this->sizeHint();
    }

    QSize
    GrowingArea::sizeHint() const
    {
      return QSize(this->_body->sizeHint().width(),
                   std::min(Super::sizeHint().height(),
                            this->minimumHeightHint()));
    }

    void
    GrowingArea::minimumHeightHint(int value)
    {
      ELLE_DUMP_SCOPE("%s: update minimum height: %s", *this, value);
      if (this->_minimumHeightHint == value)
        return;

      this->_minimumHeightHint = value;
      this->resize(this->sizeHint());
      if (this->size().height() == 0)
      {
        ELLE_DEBUG("hide");
        this->hide();
      }
      else
      {
        if (this->isHidden())
        {
          ELLE_DEBUG("show");
          this->show();
        }
      }
      emit minimumHeightHintChanged();
      ELLE_DUMP("new size: %s", this->size());
    }
  }
}
