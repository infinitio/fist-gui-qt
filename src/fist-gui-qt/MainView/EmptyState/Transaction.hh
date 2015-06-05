#ifndef FIST_GUI_QT_MAINVIEW_EMPTYSTATE_TRANSACTION_HH
# define FIST_GUI_QT_MAINVIEW_EMPTYSTATE_TRANSACTION_HH

# include <fist-gui-qt/MainView/EmptyState.hh>
# include <fist-gui-qt/onboarding/ImageOnboarder.hh>

namespace fist
{
  namespace mainview
  {
    namespace empty_state
    {
      class Transaction
        : public EmptyState
      {
        typedef EmptyState Super;
      public:
        Transaction(QWidget* parent = nullptr);

      protected:
        void
        _run_tutorial() override;
      };
    }
  }
}

#endif
