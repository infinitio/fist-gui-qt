#include <memory>

#include <QDesktopWidget>
#include <QFont>
#include <QIcon>
#include <QPixmap>
#include <QTextCodec>

#include <elle/os/environ.hh>
#include <elle/os/path.hh>
#include <elle/log.hh>

#include <common/common.hh>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>
#include <fist-gui-qt/Updater.hh>
#include <fist-gui-qt/InfinitDock.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Fist");

class Fist::Prologue
{
  friend Fist;

  Prologue()
  {
    if (!elle::os::inenv("ELLE_LOG_LEVEL"))
    {
      elle::os::setenv("ELLE_LOG_LEVEL",
                       "*surface*:DEBUG,*frete*:TRACE,*station*:DEBUG,*FIST*:TRACE",
                       true);
    }

    // if (!elle::os::inenv("INFINIT_LOG_FILE"))
    // {
    //   elle::os::setenv(
    //     "ELLE_LOG_FILE",
    //     elle::os::path::join(common::infinit::home(), "state.log"),
    //     true);
    // }

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
  _application(new QApplication(argc, argv)),
  _updater(nullptr),
  _login_window(nullptr),
  _dock(nullptr),
  _state(gap_state())
{
  ELLE_TRACE_SCOPE("%s: construction", *this);

  connect(this->_application.get(), SIGNAL(aboutToQuit()),
          this, SLOT(about_to_quit()));

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

void
Fist::logged_in()
{
  this->_login_window->hide();

  this->_dock = new InfinitDock(this->_state);
      this->_dock->show();

      connect(this->_dock, SIGNAL(quit_request()),
      this, SLOT(quit()));

      //  this->_login_window->close();
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
