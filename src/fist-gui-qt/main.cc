#include <iostream>

#include <stdexcept>

#include <fist-gui-qt/Fist.hh>

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
