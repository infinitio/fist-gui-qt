#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <QScrollArea>
# include <QFrame>

# include <surface/gap/gap.h>

# include <set>
# include <unordered_map>
# include <algorithm>

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
  addTransaction(gap_State* state, uint32_t tid);

  void
  on_transaction_accepted(uint32_t);

  void setFocus();

  void
  on_list_resized();

  void
  updateTransaction(gap_State* state,
                    uint32_t tid);

private:
  ListWidget* _list;
  gap_State* _state;

public:
  TransactionFooter*
  footer();

  std::unordered_map<uint32_t, TransactionModel> _transactions;

private:
  Q_OBJECT;

};

#endif
