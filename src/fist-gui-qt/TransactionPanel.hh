#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <set>
# include <unordered_map>
# include <algorithm>

# include <QScrollArea>
# include <QFrame>

# include <surface/gap/gap.hh>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/TransactionFooter.hh>
# include <fist-gui-qt/TransactionWidget.hh>
# include <fist-gui-qt/Panel.hh>

class TransactionPanel:
  public Panel
{
public:
  TransactionPanel(gap_State* state, QWidget* parent = nullptr);
  static void transaction_cb(uint32_t id, gap_TransactionStatus status);

public Q_SLOTS:
  TransactionWidget*
  add_transaction(gap_State* state, uint32_t tid, bool init = false);

  void
  setFocus();

  void
  avatar_available(uint32_t uid);

  void
  user_status_changed(uint32_t uid, gap_UserStatus status);

private Q_SLOTS:
  void
  _on_transaction_accepted(uint32_t);

  void
  _on_transaction_rejected(uint32_t);

  void
  _on_transaction_canceled(uint32_t);

public:
  void
  updateTransaction(gap_State* state,
                    uint32_t tid);

signals:
  void
  new_transaction(uint32_t id);

  void
  new_transaction_shown(TransactionWidget* widget);

private:
  ListWidget* _list;
  gap_State* _state;

public:
  TransactionFooter*
  footer();

  void
  _transaction_cb(uint32_t id,
                  gap_TransactionStatus status);

private:
  std::unordered_map<uint32_t, std::unique_ptr<TransactionModel>> _transactions;

private:
  Q_OBJECT;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
