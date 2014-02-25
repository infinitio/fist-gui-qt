#ifndef LOGINWINDOW_HH
# define LOGINWINDOW_HH

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/InfinitDock.hh>
# include <fist-gui-qt/RoundShadowWidget.hh>

class LoginWindow:
  public RoundShadowWidget
{
  typedef RoundShadowWidget Super;

private:
  gap_State* _state;
public:
  LoginWindow(gap_State* state);
  ~LoginWindow();

  void keyPressEvent(QKeyEvent* event);

private slots:
  void
  _quit();

  void
  _reduce();

  void
  _login();

private:
  void
  focusInEvent(QFocusEvent* event) override;

private:
  QLineEdit* _email_field;
  QLineEdit* _password_field;
  QLabel* _message_field;

  IconButton* _quit_button;
  QLabel* _reset_password_link;
  QLabel* _create_account_link;

  QLabel* _version_field;

  bool _is_logging;

public Q_SLOTS:
  void
  set_message(QString const&);

  void
  set_version();

/*----------.
| Printable |
`----------*/
private:
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT
};

#endif // !LOGINWINDOW_HH
