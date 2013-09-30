#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <QScrollArea>
# include <QFrame>
# include <surface/gap/gap.h>

# include "TransactionList.hh"

class TransactionPanel:
  public QWidget
{
public:
  TransactionPanel();

public Q_SLOTS:
  TransactionWidget*
  addTransaction(gap_State* state, uint32_t tid);

  void setFocus();

private:
  TransactionList* _list;
  Q_OBJECT;
};

#endif
