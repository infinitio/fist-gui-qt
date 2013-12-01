#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <QScrollArea>
# include <QFrame>
# include <surface/gap/gap.h>

# include <set>
# include <unordered_map>
# include <algorithm>

# include <fist-gui-qt/TransactionList.hh>

class TransactionPanel:
  public QWidget
{
public:
  TransactionPanel(gap_State* state, QWidget* parent = nullptr);
  static void transaction_cb(uint32_t id, gap_TransactionStatus status);

public Q_SLOTS:
  TransactionWidget*
  addTransaction(gap_State* state, uint32_t tid);

  void setFocus();

private:
  TransactionList* _list;
  gap_State* _state;
  Q_OBJECT;
};

#endif
