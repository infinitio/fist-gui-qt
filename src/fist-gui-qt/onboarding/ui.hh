#ifndef FIST_GUI_QT_ONBOARDING_UI_HH
# define FIST_GUI_QT_ONBOARDING_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace onboarding
  {
    namespace view
    {
      namespace landing
      {
        namespace title
        {
          static
          fist::style::Text const
          style(fist::Font(SourceSans, 15, QFont::Bold), QColor(0x2A, 0x2A, 0x2A));
        }

        namespace body
        {
          static
          fist::style::Text const
          style(fist::Font(SourceSans, 13), QColor(0x8A, 0x8A, 0x8A));
        }

        namespace no_limits
        {
          static const QString
          title("No Limits");

          static const QString
          image(":/onboarding/nolimits");

          static const QString
          body("Send files and folders of any size or format, from a "
               "complete photo collection, to a HD movie.");
        }

        namespace speed
        {
          static const QString
          image(":/onboarding/fast");

          static const QString
          title("Lightning Speed");

          static const QString
          body("Up to 30 times faster than existing solutions thanks to a "
               "unique point-to-point technology.");
        }

        namespace unstoppable
        {
          static const QString
          image(":/onboarding/unstoppable");

          static const QString
          title("Unstoppable");

          static const QString
          body("Disconnecting from the Internet will never stop your "
               "transfers, ensuring that your files are delivered no matter "
               "what.");
        }

        namespace secure
        {
          static const QString
          image(":/onboarding/secure");

          static const QString
          title("Secure");

          static const QString
          body("Your files are protected from start to finish through "
               "bank-level encryption algorithms for maximum security.");
        }
      }

      namespace screen
      {
        namespace title
        {
          static
          fist::style::Text const
          style(fist::Font(SourceSans, 24), QColor(0x2A, 0x2A, 0x2A));
        }

        namespace body
        {
          static
          fist::style::Text const
          style(fist::Font(SourceSans, 14), QColor(0x8A, 0x8A, 0x8A));
        }
      }

      namespace send_to_email
      {
        static const QString
        image(":/onboarding/send_to_an_email_address");

        static const QString
        title("Send to an email address");

        static const QString
        body("Simply open Infinit, click the paper airplane button at the "
             "bottom right of the application, add files, enter the "
             "recipient's <a style=\"color: #000000;\">email address</a> and click 'Send'.<br><br><br>"
             "The recipient will receive an email with options to <a style=\"color: #000000;\">download the "
             "files directly</a> or install Infinit.");
      }

      namespace send_to_infinit_user
      {
        static const QString
        image(":/onboarding/send_to_a_contact");

        static const QString
        title("Send to other Infinit users");

        static const QString
        body("With your friends on Infinit, you can send files and folders of "
             "<a style=\"color: #000000;\">any size for free</a> without having to enter an <a style=\"color: #000000;\">email address</a>.<br><br><br>"
             "Oh, and it's <a style=\"color: #000000;\">super secure and way faster</a> than services like "
             "email, Dropbox, WeTransfer etc.");
      }

      namespace receive
      {
        static const QString
        image(":/onboarding/accept");

        static const QString
        title("Receive from Infinit users");

        static const QString
        body("Receiving files with one click. Just open Infinit and <a style=\"color: #000000;\">click the "
             "'Accept' button</a> to start the transfer.<br><br><br>"
             "You'll even be notified when the files are available.");
      }

      namespace send_to_self
      {
        static const QString
        image(":/onboarding/send_to_self");

        static const QString
        title("Transfer file between your own devices");

        static const QString
        body("Install Infinit on your other devices and transfer photos, "
             "videos, documents and more to your <a style=\"color: #000000;\">tablet or smartphone</a> in the "
             "blink of an eye.<br><br><br>"
             "Available on Windows, Mac, Linux, iOS and Android.");
      }

      namespace always_available
      {
        static const QString
        image(":/onboarding/always_available");

        static const QString
        title("Always Available");

        static const QString
        body("Make Infinit <a style=\"color: #000000;\">always visible</a> by clicking Customize in the in the "
             "system tray pop-up.<br><br><br>Then select <a style=\"color: #000000;\">Show icon and notifications for Infinit</a>.");
      }
    }
  }
}

#endif
