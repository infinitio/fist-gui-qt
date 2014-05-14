#include "_Tab.hh"

# include <fist-gui-qt/gui/Tab.hh>
# include <fist-gui-qt/gui/Tabs.hh>

_Tab::_Tab(Tab const& tab,
           QString const& name)
  : Super(name)
  , _tab(tab)
{
}
