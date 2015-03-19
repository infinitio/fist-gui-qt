#ifndef FIST_GUI_QT_GHOST_CODE_FOOTER_HH
# define FIST_GUI_QT_GHOST_CODE_FOOTER_HH

# include <QPushButton>

# include <fist-gui-qt/Footer.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace ghost_code
  {
    class Footer
      : public ::Footer
    {
      typedef ::Footer Super;

    public:
      Footer();

      virtual
      ~Footer() = default;

      ELLE_ATTRIBUTE_R(QPushButton*, skip);
      ELLE_ATTRIBUTE_X(QPushButton*, next);
    };
  }
}


#endif
