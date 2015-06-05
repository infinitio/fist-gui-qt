#ifndef FIST_GUI_QT_MAINVIEW_EMPTYSTATE_LINK_HH
# define FIST_GUI_QT_MAINVIEW_EMPTYSTATE_LINK_HH

# include <fist-gui-qt/MainView/EmptyState.hh>

namespace fist
{
  namespace mainview
  {
    namespace empty_state
    {
      class Link
        : public EmptyState
      {
        typedef EmptyState Super;
      public:
        Link(QWidget* parent = nullptr);

      protected:
        void
        _run_tutorial() override;
      };
    }
  }
}

#endif
