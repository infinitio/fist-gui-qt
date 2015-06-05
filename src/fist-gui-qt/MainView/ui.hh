#ifndef FIST_GUI_QT_MAINVIEW_UI_HH
# define FIST_GUI_QT_MAINVIEW_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace mainview
  {
    namespace view
    {
      namespace title
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 12), QColor(0x8A, 0x8A, 0x8A));
      }

      namespace body
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 10, QFont::Bold), QColor(0x10, 0x10, 0x10));
      }

      namespace link
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 10, QFont::Bold), QColor(0x8A, 0xDF, 0xFA));
      }
    }
  }
}

#endif
