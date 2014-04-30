#include <fist-gui-qt/settings.hh>

namespace fist
{
  Settings&
  settings()
  {
    static Settings settings;
    return settings;
  }
}
