#ifndef TRANSACTIONLIST_HH
# define TRANSACTIONLIST_HH

# include <QWidget>

# include "fwd.hh"

class TransactionList:
  public QWidget
{
  public:
    Q_OBJECT;

  public Q_SLOTS:
    TransactionWidget*
    addTransaction(QString const& avatar);

  public:
    TransactionList();
  protected:
    virtual
    bool
    event(QEvent*) override;

  private:
    ListWidget* _list;
};

#endif
