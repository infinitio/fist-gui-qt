#ifndef LOGINWINDOW_HH
# define LOGINWINDOW_HH

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>

# include <surface/gap/gap.h>
# include "InfinitDock.hh"
#include <iostream>

class LoginWindow:
  public QMainWindow
{
  Q_OBJECT

public:
  LoginWindow(gap_State* state);
  void keyPressEvent(QKeyEvent* event);

public slots:
  void login();

private:
  QLineEdit* _login;
  QLineEdit* _pw;
  QPushButton* _button;
  QLabel* _msg;
  QVBoxLayout* _layout;

  gap_State* _state;
};

#endif // !LOGINWINDOW_HH
