#ifndef DEBUGWINDOW_HH
# define DEBUGWINDOW_HH

# include <QMainWindow>

class DebugWindow:
  public QMainWindow
{
public:
  Q_OBJECT;

public:
  DebugWindow();

Q_SIGNALS:
  void addTransaction(QString const& avatar);
  void onProgressChanged(float progress);

private Q_SLOTS:
  void _add_transaction();
  void _change_progress(int value);
};

#endif
