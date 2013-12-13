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
# include <fist-gui-qt/RoundButton.hh>

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

  void
  update() override;

public slots:
  void accept();
  void reject();
  void cancel();
  void update_progress();
  void update_status();

private:
  TransactionModel const& _transaction;
  AvatarWidget* _peer_avatar;
  QLabel* _peer_status;
  QLayout* _layout;
  RoundButton* _accept_button;
  RoundButton* _reject_button;
  RoundButton* _cancel_button;
  QLabel* _mtime;
  QLabel* _status;
  QTimer* _timer;
};

#endif
