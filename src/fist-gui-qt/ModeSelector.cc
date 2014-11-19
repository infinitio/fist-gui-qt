#include <fist-gui-qt/ModeSelector.hh>

namespace fist
{
  ModeSelector::ModeSelector(Mode default_mode)
    : _mode(default_mode)
  {
  }

  void
  ModeSelector::mode(Mode mode)
  {
    if (mode != this->_mode)
    {
      this->_mode = mode;
      this->_mode_implementation();
    }
  }
}
