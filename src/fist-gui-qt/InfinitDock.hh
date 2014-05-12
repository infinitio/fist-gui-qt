#ifndef INFINITDOCK_HH
# define INFINITDOCK_HH

# include <memory>

# include <QFileDialog>
# include <QFocusEvent>
# include <QMainWindow>
# include <QSystemTrayIcon>
# include <QTimer>
# include <QWidget>

# include <elle/attribute.hh>

# include <surface/gap/enums.hh>
# include <surface/gap/fwd.hh>

# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/fwd.hh>

class InfinitDock:
  public RoundShadowWidget
{
  // This class is used to create tasks that need to be done first.
  // Using the same pattern than pimpl makes it easy to update without polluting
  // the api.
private:
  class Prologue;
  std::unique_ptr<Prologue> _prologue;
  ELLE_ATTRIBUTE(fist::State, state);

/*------.
| Types |
`------*/
public:
  typedef RoundShadowWidget Super;

/*-------------.
| Construction |
`-------------*/
public:
  InfinitDock(gap_State* state);

/*------------.
| Destruction |
`------------*/
public:
  ~InfinitDock();

signals:
  void
  quit_request();

public:
  static void connection_status_cb(gap_UserStatus const status);
  static void user_status_cb(uint32_t id, gap_UserStatus const status);
  static void avatar_available_cb(uint32_t id);

public slots:
  void _update();

public slots:
  void _systray_activated(QSystemTrayIcon::ActivationReason reason);

Q_SIGNALS:
  void avatar_available(uint32_t id);
  void user_status_changed(uint32_t, gap_UserStatus);
  void connection_status_changed(gap_UserStatus);

/*------.
| Panel |
`------*/
public Q_SLOTS:
  TransactionPanel&
  transactionPanel();

  void
  _register_panel(Panel* panel);

  void
  toggle_dock(bool toogle_only = false);

  void pick_files();

  void
  report_a_problem();

  void _position_panel();

public slots:
  void
  _show_send_view();

  void
  _show_user_view(uint32_t sender_id);

  void
  _show_transactions_view();

  void
  _show_menu();

  void
  _back_from_send_view();

  void
  _systray_message_clicked();

private:
  void
  showEvent(QShowEvent* event) override;

  void
  hideEvent(QHideEvent* event) override;

private:
  void
  focusOutEvent(QFocusEvent* event) override;

private:
  void _switch_view(Panel* target);

  ELLE_ATTRIBUTE(TransactionPanel*, transaction_panel);
//  RoundShadowWidget* _panel;

private Q_SLOTS:
  void
  _systray_message(QString const& title,
                   QString const& message,
                   QSystemTrayIcon::MessageIcon icon);

private:
  SendPanel* _send_panel;

/*-----.
| Menu |
`-----*/
private:
  QMenu* _menu;

public:
  virtual
  void
  keyPressEvent(QKeyEvent* event) override;

protected:
  virtual
  void
  closeEvent(QCloseEvent* event) override;

  virtual
  void
  mouseReleaseEvent(QMouseEvent* event) override;

  virtual
  void
  enterEvent(QEvent* event) override;

  void
  focusInEvent(QFocusEvent* event) override;

private:
  QPixmap _logo;
  QSystemTrayIcon* _systray;
  QMenu* _systray_menu;
  QAction* _send_files;
  QAction* _report_a_problem;
  QAction* _quit;
  ELLE_ATTRIBUTE(std::unique_ptr<fist::onboarding::Onboarder>, onboarder);

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

  /*-----------.
  | Friendship |
  `-----------*/
  friend fist::onboarding::Onboarder;

  /*-----------.
  | Onboarding |
  `-----------*/
#ifndef FIST_PRODUCTION_BUILD
private:
  QAction* _start_onboarding_action;
#endif

private slots:
  void
  _start_onboarded_reception();

  void
  _start_onboarded_sending();

private slots:
  void
  _on_onboarded_reception_completed();

  void
  _on_onboarded_sending_completed();

private:
  Q_OBJECT
};

inline
std::ostream&
operator << (std::ostream& out, QSystemTrayIcon::ActivationReason const& reason)
{
  switch (reason)
  {
    case QSystemTrayIcon::Unknown:
      out << "Unknown reason"; break;
    case QSystemTrayIcon::Context:
      out << "Context menu requested"; break;
    case QSystemTrayIcon::DoubleClick:
      out << "Double clicked"; break;
    case QSystemTrayIcon::Trigger:
      out << "Triggered"; break;
    case QSystemTrayIcon::MiddleClick:
      out << "MiddleClicked"; break;
  }
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, Qt::FocusReason reason)
{
  switch (reason)
  {
    case Qt::MouseFocusReason:
      out << "mouse";
      break;
    case Qt::TabFocusReason:
      out << "tab";
      break;
    case Qt::BacktabFocusReason:
      out << "backtab";
      break;
    case Qt::ActiveWindowFocusReason:
      out << "active window";
      break;
    case Qt::PopupFocusReason:
      out << "popup";
      break;
    case Qt::ShortcutFocusReason:
      out << "shortcut";
      break;
    case Qt::MenuBarFocusReason:
      out << "menu bar";
      break;
    case Qt::OtherFocusReason:
      out << "other reason";
      break;
    default:
      out << "no reason";
  }
  return out << " focus";
}

#endif
