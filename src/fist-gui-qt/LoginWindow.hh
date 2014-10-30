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
# include <fist-gui-qt/LoginThread.hh>

class LoginWindow:
  public RoundShadowWidget
{
  typedef RoundShadowWidget Super;

private:
  ELLE_ATTRIBUTE(fist::State&, state);
public:
  LoginWindow(fist::State& state,
              bool fill_email_and_password_fields = true);
  ~LoginWindow();

  void keyPressEvent(QKeyEvent* event);

private slots:
  void
  _reduce();

  void
  _login(bool is_auto = false);

  void
  _login_attempt(gap_Status status);

public slots:
  void
  try_auto_login();

Q_SIGNALS:
  void
  logged_in();

  void
  quit_request();

  void
  version_rejected();

  void
  login_attempt();

  void
  login_failed();

public slots:
  void
  update_available(bool mandatory,
                   QString const& changelog);

  void
  download_progress(qint64 downloaded,
                    qint64 total_size);

  void
  download_ready();

Q_SIGNALS:
  void
  update_application();

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

  ELLE_ATTRIBUTE(fist::LoginThread*, login_thread);
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

  void
  cancel_login();

private slots:
  void
  _clear_login_thread();

private:
  // Save the password into user settings.
  void
  _save_password(QString const& email,
                 QString const& password);

  // Get the password from settings.
  QString
  _saved_password(QString const& email) const;
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
