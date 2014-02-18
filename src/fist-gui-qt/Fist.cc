#include <memory>

#include <QDesktopWidget>
#include <QFont>
#include <QIcon>
#include <QPixmap>
#include <QTextCodec>

#include <elle/log.hh>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Fist");

Fist::Fist(int argc, char** argv):
  _application(new QApplication(argc, argv)),
  _state(gap_new())
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

int
Fist::operator ()()
{
  ELLE_TRACE_SCOPE("%s: run application", *this);

  // Qt handle destruction.
  LoginWindow* login = new LoginWindow(this->_state);

  ELLE_DEBUG("show login window");

  login->show();

  return this->_application->exec();
}

void
Fist::print(std::ostream& stream) const
{
  stream << "Fist";
}
