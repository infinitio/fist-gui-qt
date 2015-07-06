#include <fist-gui-qt/MainView/EmptyState/Transaction.hh>
#include <fist-gui-qt/onboarding/ImageOnboarder.hh>

namespace fist
{
  namespace mainview
  {
    namespace empty_state
    {
      Transaction::Transaction(QWidget* parent)
        : Super(":/onboarding/transaction",
                "No current transfer.",
                "Send your first file!",
                parent)
      {
      }

      void
      Transaction::_run_tutorial()
      {
        auto* window = new onboarding::ImageOnboarder(nullptr);
        window->show();
      }
    }
  }
}
