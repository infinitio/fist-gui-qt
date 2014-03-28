#include <stdexcept>

#include <QApplication>

#include <elle/Plugin.hh>
#include <elle/log.hh>

#include <reactor/logger.hh>

#include <surface/gap/gap.h>

#include <fist-gui-qt/Fist.hh>
#include <fist-gui-qt/LoginWindow.hh>

elle::PluginLoad load_reactor_logger_plugins(
  reactor::plugins::logger_indentation,
  reactor::plugins::logger_tags
  );

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
