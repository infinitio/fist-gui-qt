#ifndef INFINITDOCK_HH
# define INFINITDOCK_HH

# include <QMainWindow>
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
  Q_OBJECT;
};

#endif
