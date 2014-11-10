#ifndef FIST_GUI_QT_LOGIN_FWD_HH
# define FIST_GUI_QT_LOGIN_FWD_HH

namespace fist
{
  namespace login
  {
    class Window;
    class Footer;

    enum class Mode
    {
      None,
      Login,
      Register,
      Loading,
    };
  }
}

#endif
