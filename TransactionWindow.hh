#ifndef TRANSACTIONWINDOW_HH
# define TRANSACTIONWINDOW_HH

# include <QMainWindow>
# include <QVBoxLayout>
# include <QKeyEvent>
# include <QLabel>

# include "Models.hh"

class TransactionWindow:
  public QMainWindow
{
public:
  TransactionWindow(Transaction const& t, QWidget* parent);
  ~TransactionWindow();
  void keyPressEvent(QKeyEvent* event);

private:
  Transaction const& _transaction;

  QLabel* _user;
  QLabel* _file;
};

#endif // !TRANSACTIONWINDOW_HH
