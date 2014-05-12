#ifndef TWOSTATEICONBUTTON_HH
# define TWOSTATEICONBUTTON_HH

# include <elle/attribute.hh>

# include <fist-gui-qt/IconButton.hh>

namespace fist
{
  class TwoStateIconButton:
    public IconButton
  {
    typedef IconButton Super;
    enum class State
    {
      released,
        released_hover,
        pressed,
        pressed_hover,
        };

  public:
    TwoStateIconButton(QPixmap const& released,
                       QPixmap const& pressed,
                       bool already_pressed = false);

  public:
    void
    enterEvent(QEvent*) override;

    void
    leaveEvent(QEvent*) override;

  private slots:
    void
    _clicked() override;

  signals:
    void
    pressed();

    void
    released();

  private:
    ELLE_ATTRIBUTE_R(State, state);
    ELLE_ATTRIBUTE(QPixmap, released_pixmap);
    ELLE_ATTRIBUTE(QPixmap, pressed_pixmap);

  private:
    Q_OBJECT;
  };
}

#endif
