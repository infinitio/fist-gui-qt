#ifndef LOGINWINDOW_HH
# define LOGINWINDOW_HH

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>

# include <surface/gap/gap.h>

# include <fist-gui-qt/ui_LoginWindow.h>
# include <fist-gui-qt/InfinitDock.hh>

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
  Q_OBJECT
};

#endif // !LOGINWINDOW_HH
