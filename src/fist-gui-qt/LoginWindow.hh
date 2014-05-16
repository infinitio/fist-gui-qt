#ifndef LOGINWINDOW_HH
# define LOGINWINDOW_HH

# include <memory>

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>
# include <QMovie>
# include <QFuture>
# include <QFutureWatcher>

# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/InfinitDock.hh>
# include <fist-gui-qt/RoundShadowWidget.hh>

class LoginWindow:
  public RoundShadowWidget
{
  typedef RoundShadowWidget Super;

private:
  ELLE_ATTRIBUTE(fist::State&, state);
public:
  LoginWindow(fist::State& state);
  ~LoginWindow();

  void keyPressEvent(QKeyEvent* event);

private slots:
  void
  _reduce();

  void
  _login();

  void
  _login_attempt();

Q_SIGNALS:
  void
  logged_in();

  void
  quit_request();

  void
  version_rejected();
private:
  void
  focusInEvent(QFocusEvent* event) override;

private:
  ELLE_ATTRIBUTE(QLineEdit*, email_field);
  ELLE_ATTRIBUTE(QLineEdit*, password_field);
  ELLE_ATTRIBUTE(QLabel*, message_field);
  ELLE_ATTRIBUTE(QMovie*, loading_icon);

  ELLE_ATTRIBUTE(IconButton*, quit_button);
  ELLE_ATTRIBUTE(QLabel*, reset_password_link);
  ELLE_ATTRIBUTE(QLabel*, create_account_link);

  ELLE_ATTRIBUTE(QLabel*, version_field);
  ELLE_ATTRIBUTE(LoginFooter*, footer);

  ELLE_ATTRIBUTE(QFuture<gap_Status>, login_future);
  ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, login_watcher);
public:
  void
  closeEvent(QCloseEvent* event) override;

public Q_SLOTS:
  void
  set_message(QString const& message,
              QString const& tooltip);

  void
  set_version();

  void
  _enable();

  void
  _disable();

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
