#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include "TransactionList.hh"

class TransactionPanel:
  public QWidget
{
public:
  TransactionPanel();

public Q_SLOTS:
  TransactionWidget*
  addTransaction(Transaction const& t);

  void setFocus();

private:
  TransactionList* _list;
  Q_OBJECT;
};

#endif
