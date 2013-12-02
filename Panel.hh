#ifndef FIST_PANEL_HH
# define FIST_PANEL_HH

# include <fist-gui-qt/SmoothLayout.hh>
# include <fist-gui-qt/Footer.hh>

class Panel:
  public SmoothLayout
{
public:
  Panel(Footer* footer,
        QWidget* owner = nullptr):
    SmoothLayout(owner),
    _footer(footer)
  {}

protected:
  Footer* _footer;

public:
  virtual
  void
  on_show() {}

  virtual
  void
  on_hide() {}

public:
  virtual
  Footer*
  footer()
  {
    // XXX: assert.
    return this->_footer;
  }
};

#endif