#include <memory>

#include <QDesktopWidget>
#include <QFont>
#include <QIcon>
#include <QLocalSocket>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QTextCodec>

#include <elle/os/environ.hh>
#include <elle/os/path.hh>
#include <elle/log.hh>
#include <elle/Exception.hh>

#include <common/common.hh>
#include <version.hh>

#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/Updater.hh>
#include <fist-gui-qt/InfinitDock.hh>

int LOG_ROTATION_SIZE = 4;
ELLE_LOG_COMPONENT("infinit.FIST.Fist");

static
int
_available_log_slot(std::string const& pattern,
                int log_rotation_size)
{
  int available_slot = 0;
  for (int i = 0; i < log_rotation_size; ++i)
  {
    if (!QFile::exists(QString::fromStdString(elle::sprintf(pattern, i))))
    {
      available_slot = i;
      break;
    }
  }
  ELLE_DEBUG("available log slot number: %s", available_slot);
  return available_slot;
}

#ifdef INFINIT_PRODUCTION_BUILD
static
std::string
_previous_log(int log_rotation_size = LOG_ROTATION_SIZE)
{
  auto pattern = elle::os::path::join(common::infinit::home(), "state%s.log");
  auto available_slot = _available_log_slot(pattern, log_rotation_size);
  // Clear next slot.
  auto prev = ((available_slot + log_rotation_size - 1) % log_rotation_size);
  ELLE_DEBUG("previous log slot number: %s", prev);
  return elle::sprintf(pattern, prev);
}
#endif

static
std::string
_rotated_log_file(int log_rotation_size = LOG_ROTATION_SIZE)
{
  auto pattern = elle::os::path::join(common::infinit::home(), "state%s.log");
  auto available_slot = _available_log_slot(pattern, log_rotation_size);
  std::string logpath = elle::sprintf(pattern, available_slot);
  // Clear next slot.
  auto next_slot =
    QString::fromStdString(
      elle::sprintf(pattern, (++available_slot % log_rotation_size)));
  ELLE_DEBUG("remove next slot: %s", next_slot);
  QFile::remove(next_slot);
  ELLE_DEBUG("log file: %s", logpath);
  return logpath;
}

class Fist::Prologue
{
  friend Fist;

  Prologue()
  {
    if (!elle::os::inenv("ELLE_LOG_LEVEL"))
    {
      ELLE_DEBUG("set ELLE_LOG_LEVEL");
      elle::os::setenv(
        "ELLE_LOG_LEVEL",
        "*surface*:DEBUG,*frete*:TRACE,*station*:DEBUG,*FIST*:TRACE",
        true);
    }

    if (!elle::os::inenv("INFINIT_LOG_FILE") &&
        !elle::os::inenv("ELLE_LOG_FILE"))
    {
      ELLE_DEBUG("set INFINIT_LOG_FILE");
      elle::os::setenv("INFINIT_LOG_FILE",
                       _rotated_log_file(),
                       true);
    }

    ELLE_TRACE_SCOPE("ininitialize Prologue");
  }
};

static
gap_State*
gap_state()
{
#ifdef INFINIT_PRODUCTION_BUILD
  return gap_configurable_new(
    "https",
    "meta.8.0.api.production.infinit.io", 80,
    "trophonius.8.0.api.production.infinit.io", 443);
#else
    return gap_new();
#endif

}

Fist::Fist(int argc, char** argv):
  _prologue(new Fist::Prologue()),
  _state(gap_state()),
  _lock(nullptr),
  _filelock(
    QString::fromStdString(
      elle::os::path::join(common::infinit::home(), "lock"))),
  _application(new QApplication(argc, argv)),
  _updater(nullptr),
  _login_window(nullptr),
  _dock(nullptr)
{
  ELLE_TRACE_SCOPE("%s: construction", *this);

  connect(this->_application.get(), SIGNAL(aboutToQuit()),
          this, SLOT(about_to_quit()));

  if (!this->_set_lock())
    throw elle::Exception("unable to set lock");

  this->_initialize_application();
}

Fist::~Fist()
{
  ELLE_TRACE_SCOPE("%s: destruction", *this);
}

void
Fist::_initialize_application()
{
  QFont arial("Arial", 11);
  arial.setStyleStrategy(QFont::PreferAntialias);
  this->_application->setFont(arial);
  this->_application->setWindowIcon(QIcon(QPixmap(":/images/logo.png")));
  this->_application->setQuitOnLastWindowClosed(false);

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void
Fist::about_to_quit()
{
  ELLE_TRACE_SCOPE("%s: run cleaning operations", *this);

  gap_free(this->_state);

  ELLE_DEBUG("cleaning done");
}

void
Fist::quit()
{
  ELLE_TRACE_SCOPE("%s: quit", *this);
  this->_application->setQuitOnLastWindowClosed(false);

  ELLE_DEBUG("close updater")
    if (this->_updater != nullptr)
    {
      this->_updater->loading_dialog()->hide();
      this->_updater->loading_dialog()->close();
    }

  ELLE_DEBUG("close login window")
    if (this->_login_window != nullptr)
    {
      this->_login_window->hide();
      this->_login_window.reset();
    }

  ELLE_DEBUG("close dock")
    if (this->_dock != nullptr)
    {
      this->_dock->hide();
      this->_dock->close();
    }

  this->_application->setQuitOnLastWindowClosed(true);
  this->_application->quit();
  ELLE_DEBUG("quit");
}

int
Fist::operator ()()
{
  ELLE_TRACE_SCOPE("%s: run application", *this);

  auto update_url = elle::os::getenv(
    "FIST_UPDATE_FILE",
    "http://download.infinit.io/windows/64/update.xml");

  this->_updater.reset(new Updater(QString::fromStdString(update_url), this));
  this->_login_window.reset(new LoginWindow(this->_state));

  connect(this->_updater.get(), SIGNAL(quit_request()),
          this, SLOT(quit()));

  connect(this->_login_window.get(), SIGNAL(quit_request()),
          this, SLOT(quit()));

  connect(this->_updater.get(), SIGNAL(update_error(QString const&, QString const&)),
          this->_login_window.get(), SLOT(set_message(QString const&, QString const&)));

  connect(this->_updater.get(), SIGNAL(no_update_available()),
          this->_login_window.get(), SLOT(set_version()));

  connect(this->_updater->loading_dialog(), SIGNAL(resized()),
          this, SLOT(_reposition_dialog()));

  connect(this->_login_window.get(), SIGNAL(version_rejected()),
          this->_updater.get(), SLOT(check_for_updates()));

  ELLE_DEBUG("show login window");

  this->_login_window->show();
  this->_login_window->setFocus();

  this->_updater->check_for_updates();

  connect(this->_login_window.get(), SIGNAL(logged_in()),
          this, SLOT(logged_in()));

  return this->_application->exec();
}

bool
Fist::_set_lock()
{
  auto locked = [this] () -> bool
    {
      if (!elle::os::inenv("INFINIT_LOG_FILE"))
      {
        ELLE_DEBUG("remove irrelevant log");
        // Remove current irrelevant log.
        QFile::remove(
          QString::fromStdString(elle::os::getenv("INFINIT_LOG_FILE")));
      }
      else
      {
        ELLE_DEBUG("no INFINIT_LOG_FILE has been set");
      }

      this->quit();
      return false;
    };

  // Strategy to have one instance running:
  // The first one create a socket and try to connect to a server named
  // *lock_name*. If he received no answer that means that probably no server is
  // running. I used probably because strange behavior may happen if the
  // application previously crashed.
  // - On linux, /tmp/*lock_name* will be created and represent a kind of lock
  // but it will stay.
  // - On Windows two local servers can listen to the same pipe at the same
  // time, but any connections will go to one of the server.
  // So if your connection is accepted, the other one will write so a lovely
  // message to make you kill yourself.
  // Otherwise, if the socket did connect to any server, that means you are
  // the only instance. So, first step is to delete the lock file (to make sure
  // to have ownership of the server name) and listen for future connection in
  // order to kick the other instances.
  // XXX: Possible race condition, if both create the server at the exact same
  // time, both socket will be rejected and they will create the servers at the
  // exact same time.
  static QString lock_name{"InfinitLocalServerLock"};
  // Local socket to check if an server (on an other instance) is running.
  this->_other_instance.reset(new QLocalSocket(this));
  // Violent but if you receive something, it's a succide signal.
  connect(this->_other_instance.get(), SIGNAL(readyRead()),
          this, SLOT(quit()));
  this->_other_instance->connectToServer(lock_name);
  if (this->_other_instance->waitForConnected(500))
  {
    // Someone accepted your connection.
    return locked();
  }
  // Create the uniqueness server.
  QLocalServer::removeServer(lock_name);
  this->_lock.reset(new QLocalServer(this));
  // Kick the other and get focus if a new connection is available.
  connect(this->_lock.get(), SIGNAL(newConnection()),
          this, SLOT(_new_local_socket_connection()));
  this->_lock->listen(lock_name);

  // Now that we ensured that we are the only one instance running, let check if
  // our lock file is present. If it's the case, that means previous session
  // crashed (or has been ctrl - c).
  // If it's a case, send a crash report and take ownership of the lock.
  if (this->_filelock.exists()) // Previous session has not be cleaned.
  {
#ifdef INFINIT_PRODUCTION_BUILD
    auto logfile = _previous_log();

    // gap_self_email could be great but the user is not logged at this stage
    // and state is not available.
    QSettings settings("Infinit.io", "Infinit");
    settings.beginGroup("Login");
    auto email = settings.value("email", "unknown").toString();

    if (!logfile.empty())
      gap_send_user_report(
        email.toStdString().c_str(),
        "Crash",
        logfile.c_str(),
        elle::sprintf("%s on %s",
# ifdef INFINIT_WINDOWS
                      "Windows",
# elif INFINIT_LINUX
                      "Linux",
# else
                      "MacOsX",
# endif
                      INFINIT_VERSION).c_str());
#else
    ELLE_WARN("Previous session crashed");
#endif
    this->_filelock.setPermissions(QFile::WriteOwner);
    this->_filelock.remove();
  }
  this->_filelock.open(QIODevice::WriteOnly);
  this->_filelock.setPermissions(QFile::ReadOwner);
  return true;
}

void
Fist::logged_in()
{
  this->_login_window->hide();

  this->_dock = new InfinitDock(this->_state);
  this->_dock->show();

  connect(this->_dock, SIGNAL(quit_request()),
          this, SLOT(quit()));
}

void
Fist::_new_local_socket_connection()
{
  if (this->_dock)
    this->_dock->show_dock();
  else if (this->_login_window)
    this->_login_window->setFocus();
  auto socket = this->_lock->nextPendingConnection();
  std::string death{"kill your self"};
  socket->write(death.c_str(), death.size());
}

void
Fist::print(std::ostream& stream) const
{
  stream << "Fist";
}

void
Fist::_reposition_dialog()
{
  auto* dialog = this->_updater->loading_dialog();
  auto* login = this->_login_window.get();

  dialog->move(login->x() + (login->width() - dialog->width()) / 2,
               login->y() + (login->height() - dialog->height()) / 2);
}
