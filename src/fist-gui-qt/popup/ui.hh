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
            url("https://infinit.io/account/referral"
                "?utm_source=app&utm_medium=windows&utm_campaign=upgrade_plan");

            static
            QString const
            text = QString(
              "You can <a style=\"text-decoration: none; color: #2BBEBD;\" href=\"%1\">"
              "upgrade your plan or invite friends</a> to remove this limitation.")
              .arg(url);
          }
        }

        namespace send_to_self
        {
          static
          QString const
          text("You have reached your monthly limit for sending files to your "
               "own devices.");

          namespace body
          {
            static
            QString const
            url("https://infinit.io/account/referral"
                "?utm_source=app&utm_medium=windows&utm_campaign=upgrade_plan");

            static
            QString const
            text = QString(
              "You can <a style=\"text-decoration: none; color: #2BBEBD;\" href=\"%1\">"
              "upgrade your plan or invite friends</a> to remove this limitation.")
              .arg(url);
          }
        }

        namespace ghost_downloads
        {
          static
          QString const
          text("%1 will need to install Infinit to get the files you sent.");

          namespace body
          {
            static
            QString const
            url("https://infinit.io/account"
                "?utm_source=app&utm_medium=windows&utm_campaign=upgrade_plan");

            static
            QString const
            text = QString(
              "You can <a style=\"text-decoration: none; color: #2BBEBD;\""
              "href=\"%1\">upgrade</a> your plan to remove this limitation.")
              .arg(url);
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

          namespace invite
          {
            static
            QString const
            text("INVITE FRIENDS");
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
