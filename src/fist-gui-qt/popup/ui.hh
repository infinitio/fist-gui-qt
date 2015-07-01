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

        namespace title
        {
          static
          fist::style::Text const
          style(fist::Font(DefaultFont, 20.5), QColor(0x51, 0x51, 0x49));
        }

        namespace link
        {
          static
          fist::style::Text const
          style(fist::Font(SourceSans, 14.5), QColor(0x51, 0x51, 0x49));
        }

        namespace storage
        {
          static
          QString const
          text("You have reached your %1 storage limit for links.");

          namespace body
          {
            static
            QString const
            url("https://infinit.io/account"
                "?utm_source=app&utm_medium=windows&utm_campaign=upgrade_plan");

            static
            QString const
            text = QString(
              "You can either <a style=\"text-decoration: none; color: #2BBEBD;\""
              "href=\"%1\">"
              "upgrade to a premium plan</a> or remove old links to free up space.")
              .arg(url);
          }
        }

        namespace too_big
        {
          static
          QString const
          text("Upgrade to send files over 2GB to an email address.");

          namespace body
          {
            static
            QString const
            text("You can upgrade to a better plan or ask your friend to create"
                 " an Infinit account to get unlimited size transfers.");
          }
        }

        namespace contact
        {
          static
          QString const
          text("Questions? Tweet us @infinit or shoot us an email at "
               "contact@infinit.io.");

          static
          fist::style::Text const
          style(fist::Font(SourceSans, 14.5), QColor(0x9D, 0x9D, 0x9D));
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
