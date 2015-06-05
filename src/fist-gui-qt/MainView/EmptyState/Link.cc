#include <fist-gui-qt/MainView/EmptyState/Link.hh>

namespace fist
{
  namespace mainview
  {
    namespace empty_state
    {
      Link::Link(QWidget* parent)
        : Super(":/onboarding/link",
                "No links yet.",
                "Create your first link!",
                parent,
                false)
      {
      }

      void
      Link::_run_tutorial()
      {
      }
    }
  }
}
