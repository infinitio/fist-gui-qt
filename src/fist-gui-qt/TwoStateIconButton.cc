# include <elle/attribute.hh>

# include <fist-gui-qt/TwoStateIconButton.hh>

namespace fist
{
  TwoStateIconButton::TwoStateIconButton(QPixmap const& released,
                                         QPixmap const& pressed,
                                         bool already_pressed)
    : IconButton(already_pressed ? pressed.scaledToHeight(20) : released.scaledToHeight(20))
    , _state(already_pressed ? State::pressed : State::released)
    , _released_pixmap(released.scaledToHeight(20))
    , _pressed_pixmap(pressed.scaledToHeight(20))
  {}

  void
  TwoStateIconButton::enterEvent(QEvent* event)
  {
    if (this->_state == State::pressed)
      this->_state = State::pressed_hover;
    else if (this->_state == State::released)
      this->_state = State::released_hover;
    Super::enterEvent(event);
  }

  void
  TwoStateIconButton::leaveEvent(QEvent* event)
  {
    if (this->_state == State::pressed_hover)
      this->_state = State::pressed;
    else if (this->_state == State::released_hover)
      this->_state = State::released;
    Super::leaveEvent(event);
  }

  void
  TwoStateIconButton::_clicked()
  {
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

    if (this->_state == State::pressed || this->_state == State::pressed_hover)
    {
      this->set_pixmap(this->_pressed_pixmap);
      emit pressed();
    }
    else
    {
      this->set_pixmap(this->_released_pixmap);
      emit released();
    }
  }
}
