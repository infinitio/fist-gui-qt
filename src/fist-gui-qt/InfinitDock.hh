
#ifndef FIST_GUI_QT_INFINITDOCK_HH
# define FIST_GUI_QT_INFINITDOCK_HH

# include <memory>

# include <QFileDialog>
# include <QFocusEvent>
# include <QMainWindow>
# include <QLabel>
# include <QSystemTrayIcon>
# include <QTimer>
# include <QWidget>

# include <elle/attribute.hh>

# include <surface/gap/enums.hh>
# include <surface/gap/fwd.hh>

# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/SystrayMessage.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/gui/SystemTrayIcon.hh>
# include <fist-gui-qt/ScreenShot/Window.hh>

class InfinitDock:
  public RoundShadowWidget
{
  // This class is used to create tasks that need to be done first.
  // Using the same pattern than pimpl makes it easy to update without polluting
  // the api.
private:
  class Prologue;
  std::unique_ptr<Prologue> _prologue;
  ELLE_ATTRIBUTE(fist::State&, state);

/*------.
| Types |
`------*/
public:
  typedef RoundShadowWidget Super;

/*-------------.
| Construction |
`-------------*/
public:
  InfinitDock(fist::State& state,
              fist::gui::systray::Icon& systray);

/*------------.
| Destruction |
`------------*/
public:
  ~InfinitDock();

public:
  void
  reset_docks();
signals:
  void
  quit_request();

  void
  logout_request();

private slots:
  void
  _on_logout();

/*------------.
| System Tray |
`------------*/
private slots:
  void
  _systray_activated(QSystemTrayIcon::ActivationReason reason);

  void
  _systray_message_clicked();

  void
  _active_transactions_changed(size_t);

/*-------.
| Update |
`-------*/
public Q_SLOTS:
  void
  update_available(bool mandatory,
                   QString const& changelog);

  void
  download_progress(qint64 downloaded, qint64 total_size);

  void
  download_ready();

signals:
  void
  update_application();

/*------.
| Panel |
`------*/
public:
  MainPanel&
  transactionPanel();

  fist::sendview::Panel&
  send_panel() const;

public slots:
  void
  _register_panel(Panel* panel);

  void
  _position_panel();

  void
  _show_send_view();

  void
  _show_ghost_code_view();

  void
  _show_user_view(uint32_t sender_id);

  void
  _show_transactions_view();

  void
  _show_menu();

  void
  _back_from_send_view();

private:
  void _switch_view(Panel* target, bool show = true);

/*-----------.
| Visibility |
`-----------*/
private:
  bool
  toggle_dock(bool toogle_only = false);

  void
  showEvent(QShowEvent* event) override;

  void
  hideEvent(QHideEvent* event) override;

  QSize
  sizeHint() const override
  {
    return QSize(326, Super::sizeHint().height());
  }

private slots:
  /// Show method depending on the source.
  /// Because Qt uses meta prog to handle signals, it makes impossible to do
  /// elegant stuff like:
  /// connect(button, SIGNAL(triggered()), [] { send_metric(...); });
  /// So instead, afaik, the only option is to create an indirection and do the
  /// common behavior at the end.
  void
  _show_from_menu();

  void
  _show_from_system_tray_click();

public:
  void
  show_from_another_instance();

private:
  void
  focusOutEvent(QFocusEvent* event) override;

  ELLE_ATTRIBUTE(std::unique_ptr<MainPanel>, transaction_panel);
  ELLE_ATTRIBUTE(std::unique_ptr<fist::sendview::Panel>, send_panel);
  ELLE_ATTRIBUTE(std::unique_ptr<fist::ghost_code::Panel>, ghost_code_panel);

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
  enterEvent(QEvent* event) override;

  void
  focusInEvent(QFocusEvent* event) override;

  void
  moveEvent(QMoveEvent* event) override
  {
    this->_position_panel();
  }

private:
  void
  _pick_files();

private slots:
  void
  _pick_files_from_menu();

  void
  _pick_files_from_sendview();

private slots:
  void
  _add_files(QList<QUrl> const&);

public slots:
  void
  p2p(QList<QUrl> const& list);

  void
  get_a_link(QList<QUrl> const& list);

private:
  QPixmap _logo;
  fist::gui::systray::Icon& _systray;
  QMenu* _systray_menu;
  QAction* _show;
  QAction* _send_files;
  QAction* _settings;
  QAction* _help;
  QAction* _send_feedback;
  QAction* _report_a_problem;
  QAction* _logout;
  QAction* _quit;
  QAction* _update;
  ELLE_ATTRIBUTE(std::unique_ptr<QDialog>, settings_window);
  ELLE_ATTRIBUTE(
    std::unique_ptr<fist::screenshot::RegionSelector>, region_selector);

private slots:
  void
  _fullscreen_screenshot();

  void
  _region_screenshot();

private slots:
  void
  _upload_screenshot(QPixmap const& pixmap);

  void
  _upload_region_screenshot();

private slots:
  void
  show_settings();

  void
  help();

  void
  send_feedback();

  void
  report_a_problem();

private:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

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
