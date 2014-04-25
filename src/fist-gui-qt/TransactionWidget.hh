#ifndef TRANSACTIONWIDGET_HH
# define TRANSACTIONWIDGET_HH

# include <memory>

# include <QWidget>
# include <QPushButton>
# include <QTimer>
# include <QLabel>

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

public:
  Q_PROPERTY(float progress
             READ progress
             WRITE setProgress
             NOTIFY onProgressChanged);

public:
  float progress() const;

public Q_SLOTS:
  void setProgress(float value);

Q_SIGNALS:
  void onProgressChanged(float);
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

public slots:
  void accept();
  void reject();
  void cancel();
  void update_progress();
  void update_status();
  void update_mtime();

private:
  TransactionModel const& _transaction;
  AvatarWidget* _peer_avatar;
  QLabel* _peer_status;
  QLayout* _layout;
  IconButton* _accept_button;
  IconButton* _reject_button;
  QWidget* _accept_reject_area;
  IconButton* _cancel_button;
  QLabel* _mtime;
  QLabel* _status;
  QWidget* _info_area;
  QTimer* _timer;
  QTimer* _mtime_updater;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
