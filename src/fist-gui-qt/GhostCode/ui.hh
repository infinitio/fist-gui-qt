#ifndef FIST_GUI_QT_GHOST_CODE_UI_HH
# define FIST_GUI_QT_GHOST_CODE_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace ghost_code
  {
    namespace view
    {
      namespace background
      {
        static
        QColor const
        color(0xF2, 0xF2, 0xF2);
      }

      namespace title
      {
        static
        QString const
        text("Invited by someone?");

        static
        fist::style::Text const
        style(fist::Font(Arial, 12, QFont::Bold),
              QColor(0x51, 0x51, 0x49));
      }

      namespace subtitle
      {
        static
        QString const
        text("Enter your code to retrieve your files.");

        static
        fist::style::Text const
        style(fist::Font(Arial, 10), QColor(0xA7, 0xA7, 0xA7));
      }

      namespace code_field
      {
        static
        fist::style::Text
        style(fist::Font(Arial, 15), QColor(0x33, 0x33, 0x33));
      }
    }
  }
}

#endif
