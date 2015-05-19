#ifndef FIST_HH
# define FIST_HH

# include <memory>
# include <vector>

# include <QApplication>
# include <QFile>
# include <QList>
# include <QObject>
# include <QLocalServer>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>
# include <fist-gui-qt/gui/SystemTrayIcon.hh>
# include <fist-gui-qt/fwd.hh>

class Fist:
  public QObject,
  public elle::Printable
{
  typedef std::unique_ptr<fist::State> StatePtr;
  typedef Fist Self;
  typedef QObject Super;
/*------------.
| Contruction |
`------------*/

private:
  void
  _parse_options(int argc, char** argv);

public:
  Fist(int argc, char** argv);
  Fist(QApplication&& application);
  virtual
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
  _initialize_state();

  void
  _initialize_application();

  void
  _initialize_login_window(bool kicked_out = false);

  // Try to set a lock using QLocalServer as a guard mechanism.
  // Return false if the lock couldn't be set.
  // _command_line is used to check if some special options has been passed via
  // the command line, like "--send C:\file.txt" so we can inform the other
  // process (if it's running) to handle the commands, if not, arguments are
  // stored to be proceed as soon as we are logged in.
  bool
  _set_uniqunness_guard();

  // Write a lock file in the .infinit folder.
  // This lock is used to detect if a previous instance of the app crashed.
  // If a lock is already present, this function will send a crash report.
  void
  _put_lock_file();

/*------------------.
| Desinitialization |
`------------------*/
public slots:
  // This slot will be linked to application 'aboutToQuit' signal in order to do
  // 'some last-second cleanup'.
  void
  about_to_quit();

  // Logout the user, spawing a new state and showing the login window.
  void
  logout(bool kicked_out = false,
         bool final = false);

  // This handle the process of killing the application.
  void
  quit();

private:
  // Destroy the state, dock and login window.
  void
  _clear();

private slots:
  void
  _kicked_out_reset();

  void
  _reset(bool kicked_out = false);

/*-----.
| Dock |
`-----*/
private slots:
  // Spawn the dock when the login is successful.
  void
  logged_in();

  // Spawn the dock when the register is successful.
  void
  registered();

  // Behavior when trying to login.
  void
  _logging_in();

  // Behavior when the login failed.
  void
  _login_failed();

private:
  std::vector<std::string>
  _extact_files_from_commandline(std::string const& message);

private slots:
  // When an other instance try to connect to us, it will write his command line
  // through the LocalSocket.
  void
  _read_local_socket();

  //
  void
  _new_local_socket_connection();

  void
  _kicked_by_another_instance();

private:
  static
  void
  _kicked_out_callback();

  bool
  eventFilter(QObject *obj, QEvent *event) override;

signals:
  void
  p2p(QList<QUrl> const& url);

  void
  get_a_link(QList<QUrl> const& url);

private slots:
  void
  _kicked_out(QString const& reason = "");

  void
  _update_loginwindow_updater_connections(bool connect);
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
  ELLE_ATTRIBUTE(StatePtr, state);
  // The local server can be used as a 'only one instance running' lock.
  // They are 3 ways usually used:
  // - QLocalServer.
  // - QSharedMemory.
  // - QtSingleApplication (deprecated).
  // See: http://developer.nokia.com/community/wiki/Run_only_one_instance_of_a_Qt_application
  typedef QLocalServer Lock;
  ELLE_ATTRIBUTE(std::unique_ptr<Lock>, lock);
  ELLE_ATTRIBUTE(bool, previous_session_crashed);
  ELLE_ATTRIBUTE(std::unique_ptr<QLocalSocket>, other_instance);
  ELLE_ATTRIBUTE(QFile, filelock);
  ELLE_ATTRIBUTE(std::unique_ptr<QApplication>, application);
  ELLE_ATTRIBUTE(std::unique_ptr<Updater>, updater);
  ELLE_ATTRIBUTE(fist::gui::systray::Icon, systray);
  ELLE_ATTRIBUTE(std::unique_ptr<fist::login::Window>, login_window);
  ELLE_ATTRIBUTE(std::unique_ptr<InfinitDock>, dock);
  // Files passed through the command line.
  // XXX: Rework that heresy someday, use rpc to communicate between intances.
  struct CommandLine
  {
    CommandLine(std::vector<std::string> const& files = std::vector<std::string>());
    QList<QUrl>
    urls() const;

    std::vector<std::string> files;
    bool link;
    bool p2p;

    ELLE_ATTRIBUTE_RW(QString, code);
    ELLE_ATTRIBUTE_RW(QString, fingerprint);

  public:
    void
    parse(std::string const& instruction);
  };

  ELLE_ATTRIBUTE(std::unique_ptr<CommandLine>, command_line);
private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
