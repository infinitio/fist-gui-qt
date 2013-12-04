#ifndef TRANSACTIONLIST_HH
# define TRANSACTIONLIST_HH

# include <QWidget>
# include <QKeyEvent>

# include <unordered_map>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/TransactionModel.hh>

class TransactionList:
  public QWidget
{
public:
  Q_OBJECT;

public Q_SLOTS:
  TransactionWidget*
  addTransaction(gap_State* state, uint32_t tid);

  void
  updateTransaction(gap_State* state,
                    uint32_t tid);

  void setFocus();
  void keyPressEvent(QKeyEvent* event);

Q_SIGNALS:
  void
  on_transaction_accepted(uint32_t);

public:
  TransactionList();

protected:
  virtual
  bool
  event(QEvent*) override;

private:
  ListWidget* _list;
  std::unordered_map<uint32_t, TransactionModel> _transactions;
};

#endif
