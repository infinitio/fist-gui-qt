#include <stdexcept>

#include <QApplication>

#include <elle/log.hh>

#include <surface/gap/gap.h>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>

int
main(int argc, char** argv)
{
  try
  {
    Fist application(argc, argv);

    return application();
  }
  catch (std::runtime_error const& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
