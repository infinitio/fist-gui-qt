#ifndef DEBUGWINDOW_HH
# define DEBUGWINDOW_HH

# include <QMainWindow>

# include <surface/gap/fwd.hh>

class DebugWindow:
  public QMainWindow
{
public:
  Q_OBJECT;

public:
  DebugWindow();

Q_SIGNALS:
  void addTransaction(gap_State* state, uint32_t tid);
  void onProgressChanged(float progress);

private Q_SLOTS:
  void _add_transaction();
  void _change_progress(int value);
};

#endif
