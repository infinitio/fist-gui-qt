#ifndef INFINITDOCK_HH
# define INFINITDOCK_HH

# include <QMainWindow>
# include <QTimer>
# include <QWidget>

# include <surface/gap/gap.h>

# include "fwd.hh"

class InfinitDock:
  public QMainWindow
{
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

public slots:
  void update();
  void switch_panel();

/*------.
| Panel |
`------*/
public Q_SLOTS:
  TransactionPanel&
  transactionPanel();
  void hidePanel();
  void showPanel();
  void togglePanel();
private Q_SLOTS:
  void _position_panel();
private:
  TransactionPanel* _transaction_panel;
  RoundShadowWidget* _panel;

/*-----.
| Send |
`-----*/
private Q_SLOTS:
  void _search(QString const&);
private:
  SendPanel* _send_panel;

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
  virtual
  void
  paintEvent(QPaintEvent* event) override;

private:
  QPixmap _logo;
  QPixmap _background;
  gap_State* _state;
  Q_OBJECT;
};

#endif
