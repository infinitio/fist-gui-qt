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
  public:
    enum class State
    {
      released,
      released_hover,
      pressed,
      pressed_hover,
    };

  public:
    TwoStateIconButton(QString const& released,
                       QString const& pressed,
                       bool already_pressed = false,
                       QWidget* parent = nullptr,
                       QSize const& size = QSize(20, 20));

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
    ELLE_ATTRIBUTE(QString, released);
    ELLE_ATTRIBUTE(QString, pressed);

  private:
    Q_OBJECT;
  };

  std::ostream&
  operator <<(std::ostream& out,
              TwoStateIconButton::State const& s);
}

#endif
