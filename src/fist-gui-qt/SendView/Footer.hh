#ifndef FIST_GUI_QT_SEND_VIEW_FOOTER_HH
# define FIST_GUI_QT_SEND_VIEW_FOOTER_HH

# include <QHBoxLayout>
# include <QSpacerItem>

# include <elle/attribute.hh>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ShapeButton.hh>
# include <fist-gui-qt/ModeSelector.hh>
# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace sendview
  {
    class Footer
      : public ::Footer
      , public ModeSelector
    {
    public:
      Footer();

      virtual
      ~Footer() = default;

    private:
      ELLE_ATTRIBUTE_R(IconButton*, back);
      ELLE_ATTRIBUTE_R(IconButton*, send);
    private:
      void
      _mode_implementation() override;
    private:
      Q_OBJECT;
    };

  }
}

#endif
