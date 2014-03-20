#ifndef FIST_HH
# define FIST_HH

# include <memory>
# include <vector>

# include <QApplication>
# include <QFile>
# include <QObject>
# include <QLocalServer>

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

/*---------------.
| Initialization |
`---------------*/
private:
  void
  _initialize_application();

  bool
  _set_lock();
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

  void
  _new_local_socket_connection();
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
  ELLE_ATTRIBUTE(std::unique_ptr<Prologue>, prologue);
  ELLE_ATTRIBUTE(gap_State*, state);
  // The local server can be used as a 'only one instance running' lock.
  // They are 3 ways usually used:
  // - QLocalServer.
  // - QSharedMemory.
  // - QtSingleApplication (deprecated).
  // See: http://developer.nokia.com/community/wiki/Run_only_one_instance_of_a_Qt_application
  typedef QLocalServer Lock;
  ELLE_ATTRIBUTE(std::unique_ptr<Lock>, lock);
  ELLE_ATTRIBUTE(std::unique_ptr<QLocalSocket>, other_instance);
  ELLE_ATTRIBUTE(QFile, filelock);
  ELLE_ATTRIBUTE(std::unique_ptr<QApplication>, application);
  ELLE_ATTRIBUTE(std::unique_ptr<Updater>, updater);
  ELLE_ATTRIBUTE(std::unique_ptr<LoginWindow>, login_window);
  ELLE_ATTRIBUTE(InfinitDock*, dock);

private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
