#ifndef FIST_GUI_QT_GUI_C_TAB_HH
# define FIST_GUI_QT_GUI_C_TAB_HH

# include <QPushButton>

# include <elle/attribute.hh>

class Tab;
class Tabber;

class _Tab:
  public QPushButton
{
  typedef QPushButton Super;
public:
  _Tab(Tab const& tab,
       QString const& name);


private:

  // void
  // paintEvent(QPaintEvent* event) override
  // {

  // }

  // QSize
  // sizeHint() const override
  // {
  //   this->_tabber->sizeHint().width() / this->_tabber->_tabs.size();
  // }

private:
  ELLE_ATTRIBUTE(Tab const&, tab);

private:
  friend Tabber;
};



#endif
