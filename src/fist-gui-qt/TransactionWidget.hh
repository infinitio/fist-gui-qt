#ifndef TRANSACTIONWIDGET_HH
# define TRANSACTIONWIDGET_HH

# include <memory>

# include <QWidget>
# include <QPushButton>
# include <QTimer>
# include <QLabel>

# include <elle/attribute.hh>

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
  transaction_accepted(uint32_t);
  void
  transaction_rejected(uint32_t);
  void
  transaction_canceled(uint32_t);
  void
  transaction_finished(uint32_t);

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

private slots:
  void _on_avatar_updated();

private:
  ELLE_ATTRIBUTE_R(TransactionModel const&, transaction);
  ELLE_ATTRIBUTE_R(AvatarWidget*, peer_avatar);
  ELLE_ATTRIBUTE_R(QLabel*, peer_fullname);
  QLabel* _peer_status;
  QLayout* _layout;
  ELLE_ATTRIBUTE_R(IconButton*, accept_button);
  ELLE_ATTRIBUTE_R(IconButton*, reject_button);
  QWidget* _accept_reject_area;
  ELLE_ATTRIBUTE_R(IconButton*, cancel_button);
  QLabel* _mtime;
  ELLE_ATTRIBUTE_R(QLabel*, status);
  QWidget* _info_area;
  ELLE_ATTRIBUTE(std::unique_ptr<QTimer>, progress_timer);
  QTimer* _mtime_updater;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
