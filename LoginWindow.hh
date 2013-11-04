#ifndef LOGINWINDOW_HH
# define LOGINWINDOW_HH

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>

# include <surface/gap/gap.h>

# include "ui_LoginWindow.h"
# include "InfinitDock.hh"

class LoginWindow:
  public QMainWindow,
  public Ui::LoginWindow
{
private:
  gap_State* _state;
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
