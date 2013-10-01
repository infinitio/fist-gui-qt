#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <QScrollArea>
# include <QFrame>
# include <surface/gap/gap.h>

# include <vector>
# include <algorithm>

# include "TransactionList.hh"

class TransactionPanel:
  public QWidget
{
public:
  TransactionPanel(gap_State* state);

public Q_SLOTS:
  TransactionWidget*
  addTransaction(gap_State* state, uint32_t tid);

  void setFocus();

private:
  TransactionList* _list;
  std::vector<uint32_t> _uids;
  Q_OBJECT;
};

#endif
