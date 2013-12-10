#ifndef TRANSACTIONWIDGET_HH
# define TRANSACTIONWIDGET_HH

# include <QWidget>
# include <QPushButton>
# include <QTimer>
# include <QLabel>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/TransactionWindow.hh>
# include <fist-gui-qt/TransactionModel.hh>

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
  void update_progress();

private:
  TransactionModel const& _transaction;
  AvatarWidget* _avatar;
  QLayout* _layout;
  QPushButton* _accept_button;
  QPushButton* _reject_button;
  QPushButton* _cancel_button;
  QLabel* _status;
  QTimer* _timer;
};

#endif
