#include <QApplication>

#include <elle/log.hh>

#include <surface/gap/gap.h>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>

int
main(int argc, char** argv)
{
  Fist application(argc, argv);

  return application();
}
