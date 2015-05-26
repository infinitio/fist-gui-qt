#ifndef FIST_GUI_QT_LOGIN_UI_HH
# define FIST_GUI_QT_LOGIN_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace login
  {
    namespace view
    {
      static
      QSize const
      size(410, 400);

      static
      Qt::WindowFlags const
      flags = Qt::FramelessWindowHint;

      // The tabs (login / register).
      namespace mode
      {
        static
        fist::style::Text const
        default_style(fist::Font(Arial, 9), QColor(0x8A, 0x8A, 0x8A));

        static
        fist::style::Text const
        selected_style(fist::Font(Arial, 9), QColor(0x33, 0x33, 0x33));

        namespace login
        {
          static
          QString const
          text("LOGIN");
        }

        namespace signup
        {
          static
          QString const
          text("SIGN UP");
        }
      }

      namespace facebook_email
      {
        namespace title
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 12, QFont::Bold),
                QColor(0x51, 0x51, 0x49));

          static
          QString const
          text("One last thing...");
        }

        namespace subtitle
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 8), QColor(0xA7, 0xA7, 0xA7));

          static
          QString const
          text("Enter your email address to create your account.");
        }
      }

      namespace separator
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 8), QColor(0xAA, 0xAA, 0xAA));

        static
        QString const
        text("or");
      }

      namespace
      {
        namespace input_field
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 11), QColor(0x33, 0x33, 0x33));

          static
          QSize const
          size(view::size.width() - 140, 38);
        }
      }

      namespace fullname
      {
        static
        fist::style::Text const
        style = input_field::style;

        static
        QString const
        placeholder("Full Name");

        static
        QSize const
        size = input_field::size;
      }

      namespace email
      {
        static
        fist::style::Text const
        style = input_field::style;

        static
        QString const
        placeholder("Email");

        static
        QSize const
        size = input_field::size;
      }

      namespace password
      {
        static
        fist::style::Text const
        style = input_field::style;

        static
        QString const
        placeholder("Password");

        static
        QSize const
        size = input_field::size;
      }

      namespace message
      {
        static
        fist::style::Text const
        error_style(fist::Font(Arial, 10.5), QColor(0xEE, 0x11, 0x11), Qt::AlignCenter);

        static
        fist::style::Text const
        warning_style(fist::Font(Arial, 10.5), QColor(0x88, 0x88, 0x88), Qt::AlignCenter);
      }

      namespace links
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 9), QColor(0x3D, 0x91, 0xD4));

        static
        Qt::TextInteractionFlags const
        interration_flags = ::links::interration_flags;

        namespace forgot_password
        {
          static
          QString const
          text("<a style=\"text-decoration: none; color: #489FCE;\" "
               "href=\"https://infinit.io/forgot_password?utm_source=app&utm_medium=windows&utm_campaign=forgot_password\">"
               "Forgot?</a>");
        }

        namespace help
        {
          static
          QString const
          text("<a style=\"text-decoration: none; color: #489FCE;\" "
               "href=\"https://infinit.io/faq?utm_source=app&utm_medium=windows&utm_campaign=help\">"
               "Help!</a>");
        }

        namespace need_an_account
        {
          static
          QString const
          text("<a style=\"text-decoration: none; color: #489FCE;\">"
               "Need an account?</a>");
        }

        namespace already_have_an_account
        {
          static
          QString const
          text("<a style=\"text-decoration: none; color: #489FCE;\">"
               "Already have an account?</a>");
        }
      }

      namespace version
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 8.5), QColor(0x88, 0x88, 0x88));
      }

      namespace login_button
      {
        static
        QSize const
        size(view::size.width() - 140, 38);

        static
        fist::style::Text const
        style(fist::Font(Arial, 11, QFont::Bold), Qt::white);

        static
        QString const
        login_text("LOGIN");

        static
        QString const
        register_text("SIGN UP");
      }
    }
  }
}

#endif
