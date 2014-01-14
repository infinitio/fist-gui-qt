#ifndef INFINITDOCK_HH
# define INFINITDOCK_HH

# include <memory>

# include <QMainWindow>
# include <QTimer>
# include <QWidget>
# include <QSystemTrayIcon>
# include <QFileDialog>

# include <surface/gap/gap.h>
# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/fwd.hh>

class InfinitDock:
  public RoundShadowWidget
{
  // This class is used to create tasks that need done first.
  // The need comes from initialization wich create some objects that have a
  // behavior in their contructor, which may require attributes from the dock.
  // Using the same pattern than pimpl makes it easy to update without polluting
  // the api.
private:
  class Prologue;
  std::unique_ptr<Prologue> _prologue;

/*------.
| Types |
`------*/
public:
  typedef QWidget Super;

/*-------------.
| Construction |
`-------------*/
public:
  InfinitDock(gap_State* state);
  static void connection_status_cb(gap_UserStatus const status);
  static void user_status_cb(uint32_t id, gap_UserStatus const status);
  static void avatar_available_cb(uint32_t id);

public slots:
  void update();

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
  TransactionPanel& transactionPanel();
  void hidePanel();
  void showPanel();
  void togglePanel(bool toogle_only = false);
  void chooseFiles();
  void quit();
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

private:
  void _switch_view(Panel* target);

  TransactionPanel* _transaction_panel;
//  RoundShadowWidget* _panel;

/*-----.
| Send |
`-----*/
private:
  SendPanel* _send_panel;

/*-----.
| Menu |
`-----*/
private:
  QMenu* _menu;

/*--------------.
| Drag and drop |
`--------------*/
protected:
  virtual
  void
  dragEnterEvent(QDragEnterEvent *event) override;
  virtual
  void
  dropEvent(QDropEvent *event) override;

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

private:
  QPixmap _logo;
  QSystemTrayIcon* _systray;
  QMenu* _systray_menu;
  QAction* _send_files;
  std::unique_ptr<QFileDialog> _choose_files;
  QAction* _quit;
  gap_State* _state;
  Q_OBJECT
};

#endif
