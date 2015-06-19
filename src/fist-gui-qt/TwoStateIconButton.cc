# include <elle/attribute.hh>
# include <elle/log.hh>

# include <fist-gui-qt/TwoStateIconButton.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TwoStateIconButton");

namespace fist
{
  TwoStateIconButton::TwoStateIconButton(QString const& released,
                                         QString const& pressed,
                                         bool already_pressed,
                                         QWidget* parent,
                                         QSize const& size)
    : IconButton(already_pressed ? pressed : released, parent, size)
    , _state(already_pressed ? State::pressed : State::released)
    , _released(released)
    , _pressed(pressed)
  {}

  void
  TwoStateIconButton::enterEvent(QEvent* event)
  {
    if (this->_state == State::pressed)
    {
      this->_state = State::pressed_hover;
    }
    else if (this->_state == State::released)
    {
      this->_state = State::released_hover;
    }
    Super::enterEvent(event);
  }

  void
  TwoStateIconButton::leaveEvent(QEvent* event)
  {
    if (this->_state == State::pressed_hover)
    {
      this->_state = State::pressed;
    }
    else if (this->_state == State::released_hover)
    {
      this->_state = State::released;
    }
    Super::leaveEvent(event);
  }

  void
  TwoStateIconButton::_clicked()
  {
    ELLE_DEBUG("%s: clicked while %s", *this, this->_state);
    if (this->_state == State::pressed_hover)
    {
      this->_state = State::released_hover;
    }
    else if (this->_state == State::released_hover)
    {
      this->_state = State::pressed_hover;
    }
    else if (this->_state == State::released)
    {
      this->_state = State::pressed;
    }
    else
    {
      this->_state = State::released;
    }

    ELLE_DEBUG("new status: %s", this->_state);
    if (this->_state == State::pressed || this->_state == State::pressed_hover)
    {
      this->set_pixmap(this->_pressed);
      emit pressed();
    }
    else
    {
      this->set_pixmap(this->_released);
      emit released();
    }
  }

  std::ostream&
  operator <<(std::ostream& out,
              TwoStateIconButton::State const& s)
  {
    switch (s)
    {
      case TwoStateIconButton::State::released:
        out << "released"; break;
      case TwoStateIconButton::State::pressed:
        out << "pressed"; break;
      case TwoStateIconButton::State::released_hover:
        out << "released (hover)"; break;
      case TwoStateIconButton::State::pressed_hover:
        out << "pressed (hover)"; break;
    }
    return out;
  }
}
