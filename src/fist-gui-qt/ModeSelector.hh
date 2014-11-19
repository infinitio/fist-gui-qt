#ifndef FIST_GUI_QT_MULTIMODE_HH
# define FIST_GUI_QT_MULTIMODE_HH

# include <QObject>

# include <elle/attribute.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  class ModeSelector
  {
  public:
    ModeSelector(Mode default_mode = Mode::p2p);

    virtual
    ~ModeSelector() = default;

    ELLE_ATTRIBUTE_Rw(Mode, mode);

  protected:
    virtual
    void
    _mode_implementation() = 0;
  };
};


#endif
