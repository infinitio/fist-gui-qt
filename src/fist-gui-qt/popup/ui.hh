#ifndef FIST_GUI_QT_POPUP_UI_HH
# define FIST_GUI_QT_POPUP_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace popup
  {
    namespace view
    {
      namespace payment
      {
        static
        QColor const
        background(0xFF, 0xFF, 0xFF);

        namespace main
        {
          static
          QString const
          text("You have exceeded your 1GB quota for links.");

          static
          fist::style::Text const
          style(fist::Font(DefaultFont, 20.5), QColor(0x51, 0x51, 0x49));
        }

        namespace link
        {
          static
          QString const
          text("You can <a style=\"text-decoration: none; color: #2BBEBD;\" "
               "href=\"https://infinit.io/plan"
               "?utm_source=app&utm_medium=windows&utm_campaign=upgrade_plan\">"
               "upgrade to a better plan on our website</a> or remove some "
               "of your old links.");

          static
          fist::style::Text const
          style(fist::Font(SourceSans, 15.5), QColor(0x51, 0x51, 0x49));
        }

        namespace contact
        {
          static
          QString const
          text("Question? Tweet us @infinit or send us an email at "
               "contact@infinit.io.");

          static
          fist::style::Text const
          style(fist::Font(SourceSans, 15.5), QColor(0x9D, 0x9D, 0x9D));
        }

        namespace button
        {
          static
          QString const
          stylesheet(
            "QPushButton {"
            "  background-color: %1;"
            "  color: white;"
            "  font: bold 13px;"
            "  border-radius: 17px;"
            "  height: 34px;"
            "  padding-left: 18px;"
            "  padding-right: 18px;"
            "}"
            "QPushButton:focused, QPushButton:hover {"
            "  color: white;"
            "  background-color: %2;"
            "}"
            );

namespace cancel
{
  static
  QString const
  text("CANCEL");
}

namespace upgrade
{
  static
  QString const
  text("UPGRADE MY PLAN");
}
        }
      }
    }
  }
}

#endif
