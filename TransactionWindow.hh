#ifndef TRANSACTIONWINDOW_HH
# define TRANSACTIONWINDOW_HH

# include <QKeyEvent>
# include <QWidget>

# include <iostream>

class TransactionWindow:
  public QWidget
{
public:
  TransactionWindow(QWidget* parent);

public:
  void keyPressEvent(QKeyEvent* event);
  void mousePressEvent(QMouseEvent* event);
};

#endif
