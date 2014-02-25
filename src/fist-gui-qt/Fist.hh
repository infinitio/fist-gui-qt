#ifndef FIST_HH
# define FIST_HH

# include <memory>
# include <vector>

# include <QApplication>
# include <QObject>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>

class Fist:
  public QObject,
  public elle::Printable
{
/*------------.
| Contruction |
`------------*/
public:
  Fist(int argc, char** argv);
  Fist(QApplication&& application);
  ~Fist();

/*---------.
| Prologue |
`---------*/
  // This class is used to create tasks that need done first.
  // Using the same pattern than pimpl makes it easy to update without polluting
  // the api.
  // In that case, it will be use to initialize the log destination, the log
  // levels, and so on...
private:
  class Prologue;
  std::unique_ptr<Prologue> _prologue;

/*---------------.
| Initialization |
`---------------*/
private:
  void
  _initialize_application();

/*------------------.
| Desinitialization |
`------------------*/
public slots:
  // This slot will be linked to application 'aboutToQuit' signal in order to do
  // 'some last-second cleanup'.
  void
  about_to_quit();

  // This handle the process of killing the application.
  void
  quit();

/*-----.
| Dock |
`-----*/
private slots:
  void
  logged_in();

/*----.
| Run |
`----*/
public:
  int
  operator ()();

private slots:
  void
  _reposition_dialog();

private:
  ELLE_ATTRIBUTE(std::unique_ptr<QApplication>, application);
  ELLE_ATTRIBUTE(std::unique_ptr<Updater>, updater);
  ELLE_ATTRIBUTE(std::unique_ptr<LoginWindow>, login_window);
  ELLE_ATTRIBUTE(InfinitDock*, dock);
  ELLE_ATTRIBUTE(gap_State*, state);
private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
