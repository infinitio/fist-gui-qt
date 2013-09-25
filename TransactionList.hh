#ifndef TRANSACTIONLIST_HH
# define TRANSACTIONLIST_HH

# include <QWidget>
# include <QKeyEvent>
# include "Models.hh"

# include "fwd.hh"

class TransactionList:
  public QWidget
{
  public:
    Q_OBJECT;

  public Q_SLOTS:
    TransactionWidget*
    addTransaction(Transaction const& t);

    void setFocus();
    void keyPressEvent(QKeyEvent* event);

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
