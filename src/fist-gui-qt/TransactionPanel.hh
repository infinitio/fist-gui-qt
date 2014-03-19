#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <memory>
# include <set>
# include <unordered_map>
# include <algorithm>

# include <QScrollArea>
# include <QFrame>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/gap.h>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/TransactionFooter.hh>
# include <fist-gui-qt/TransactionWidget.hh>
# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/fwd.hh>

class TransactionPanel:
  public Panel
{
public:
  TransactionPanel(gap_State* state, QWidget* parent = nullptr);
  ~TransactionPanel();

  static void transaction_cb(uint32_t id, gap_TransactionStatus status);

public Q_SLOTS:
  TransactionWidget*
  add_transaction(TransactionModel const& model,
                  bool init = false);

  void
  setFocus();

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

private:
  ELLE_ATTRIBUTE(gap_State*, state);
  ELLE_ATTRIBUTE_Rw(UserModel const*, peer);
  ELLE_ATTRIBUTE(std::unique_ptr<UserWidget>, peer_widget);
  ELLE_ATTRIBUTE(std::unique_ptr<ListWidget>, list);

public:

/*------------.
| Show | Hide |
`------------*/
  void
  on_show() override;

  void
  on_hide() override;

public:
  TransactionFooter*
  footer();

  void
  _transaction_cb(uint32_t id,
                  gap_TransactionStatus status);

private:
  Q_OBJECT;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
