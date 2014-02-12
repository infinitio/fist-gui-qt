#include <QApplication>

#include <elle/log.hh>

#include <surface/gap/gap.h>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>

ELLE_LOG_COMPONENT("infinit.FIST");

int
main(int argc, char** argv)
{
  ELLE_LOG("start FIST gui");

  Fist application(argc, argv);

  return application();
}
