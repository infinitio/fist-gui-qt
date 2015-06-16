#ifndef FIST_GUI_QT_PROXY_UI_HH
# define FIST_GUI_QT_PROXY_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace proxy
  {
    namespace view
    {
      namespace section
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 11), QColor(0x13, 0x13, 0x13));
      }

      namespace line_edit
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 11), QColor(0x13, 0x13, 0x13));
      }

      namespace button
      {
        static
        QString const
        stylesheet(
          "QPushButton {"
          "  background-color: rgb(255, 255, 255);"
          "  color: black;"
          "  border: 1px solid rbg(0, 0, 0);"
          "  font: bold 13px;"
          "  border-radius: 13px;"
          "  padding-top: 5px;"
          "  padding-bottom: 5px;"
          "  padding-left: 16px;"
          "  padding-right: 16px;"
          "} ");
      }

    }
  }
}


#endif
