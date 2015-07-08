#ifndef FIST_GUI_QT_SETTINGS_UI_HH
# define FIST_GUI_QT_SETTINGS_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace prefs
  {
    namespace view
    {
      namespace manage_email_addresses
      {
        static
        QString const
        text = ::view::links::rich.arg(
          "%1",
          "You can change on the web.");
      }

      namespace account
      {
        static
        QString const
        text = ::view::links::rich.arg(
          "%1",
          "Go to your web profile.");
      }

      namespace title
      {
        static
        fist::style::Text const
        style(fist::Font(SourceSans, 11, QFont::Bold), QColor(0x13, 0x13, 0x13));
      }

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

      namespace
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
