#ifndef TRANSACTIONWIDGET_HH
# define TRANSACTIONWIDGET_HH

# include <memory>

# include <QWidget>
# include <QPushButton>
# include <QTimer>
# include <QLabel>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/TransactionWindow.hh>
# include <fist-gui-qt/TransactionModel.hh>
# include <fist-gui-qt/IconButton.hh>

class TransactionWidget:
  public ListItem
{
public:
  TransactionWidget(TransactionModel const& model);
  Q_OBJECT;

/*-----------.
| Properties |
`-----------*/

Q_SIGNALS:
  void
  on_transaction_accepted(uint32_t);
  void
  on_transaction_rejected(uint32_t);
  void
  on_transaction_canceled(uint32_t);

/*-------.
| Layout |
`-------*/

public:
  virtual
  QSize
  sizeHint() const override;

  virtual
  QSize
  minimumSizeHint() const override;

  virtual
  void
  trigger();

public:
  void
  _update() override;

private:
  void
  _show_accept_reject();

  void
  _hide_accept_reject();

public slots:
  void accept();
  void reject();
  void cancel();
  void update_progress();
  void update_status();
  void update_mtime();

private:
  TransactionModel const& _transaction;
  QLayout* _layout;
  IconButton* _accept_button;
  IconButton* _reject_button;
  IconButton* _cancel_button;
  QLabel* _mtime;
  QLabel* _status;
  QTimer* _timer;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
