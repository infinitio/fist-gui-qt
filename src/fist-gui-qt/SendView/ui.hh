#ifndef FIST_GUI_QT_SEND_VIEW_UI_HH
# define FIST_GUI_QT_SEND_VIEW_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace sendview
  {
    namespace view
    {
      namespace file
      {
        namespace name
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
        }

        namespace size
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
        }

        static
        QColor const
        background(0xF8, 0xF8, 0xF8);
      }

      namespace file_adder
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);

        static
        fist::style::Text const
        hover_style(fist::Font(Arial, 9), QColor(0x3C, 0x91, 0xD4), Qt::AlignVCenter);

        static
        QColor const
        background(0xF8, 0xF8, 0xF8);

        static
        QColor const
        pulse(0xBC, 0xD2, 0xD6, 0x77);

        static
        QString const
        text("Add files...");
      }

      namespace search_field
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 9), QColor(0x25, 0x25, 0x25), Qt::AlignVCenter);

        static
        QString const
        text("Search or select a user...");
      }

      namespace no_result
      {
        namespace title
        {
          static
          QString const
          text("No one here by that name.");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0x51, 0x51, 0x49));
        }

        namespace subtitle
        {
          static
          QString const
          text("Try an email instead.");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9, QFont::Bold),
                QColor(0x51, 0x51, 0x49));
        }
      }

      namespace too_many_results
      {
        namespace title
        {
          static
          QString const
          text("Too many results");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9, QFont::Bold), QColor(0x51, 0x51, 0x49));
        }

        namespace subtitle
        {
          static
          QString const
          text("Search filter your contacts using the search field");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0x51, 0x51, 0x49));
        }
      }

      namespace message
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 9), QColor(0x25, 0x25, 0x25), Qt::AlignVCenter);

        static
        QString const
        text("Optional message...");

        namespace remaining_characters
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 7), QColor(0xA5, 0xA5, 0xA5), Qt::AlignVCenter);

        }
      }

      namespace user
      {
        namespace fullname
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0x25, 0x25, 0x25));
        }

        namespace handle
        {
          static
          fist::style::Text const
          style(fist::Font(Arial, 8), QColor(0xC4, 0xC4, 0xC4));
        }
      }

      namespace add_files
      {
        namespace title
        {
          static
          QString const
          text("DROP FILES HERE");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9, QFont::Bold), QColor(0x51, 0x51, 0x49));
        }

        namespace subtitle
        {
          static
          QString const
          text("or click to add files...");

          static
          fist::style::Text const
          style(fist::Font(Arial, 9), QColor(0xB3, 0xB3, 0xB3));
        }
      }

    }
  }
}


#endif
