#ifndef FIST_GUI_QT_NOTIFICATION_UI_HH
# define FIST_GUI_QT_NOTIFICATION_UI_HH

# include <fist-gui-qt/globals.hh>

namespace fist
{
  namespace notification
  {
    namespace view
    {
      static const QColor background(241, 241, 241);
      static const int spacing = 15;

      namespace icon
      {
        static const int diameter = 64;
      }

      namespace title
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 10, QFont::Bold), QColor(0x13, 0x13, 0x13));
      }

      namespace body
      {
        static
        fist::style::Text const
        style(fist::Font(Arial, 10), QColor(0x13, 0x13, 0x13));
      }
    }

    namespace update_available
    {
      namespace view
      {
        namespace title
        {
          static
          fist::style::Text const
          style = view::title::style;

          static
          QString const
          text("Update infinit!");
        }

        namespace body
        {
          static
          fist::style::Text const
          style = view::body::style;

          static
          QString const
          text("Update %1 is available.");
        }
      }
    }

    namespace incoming_transaction
    {
      namespace view
      {
        namespace body
        {
          static
          fist::style::Text const
          style = view::body::style;

          QString const
          text("%1 wants to send you %2.");
        }
      }
    }

    namespace transaction_updated
    {
      namespace view
      {
        namespace title
        {
          static
          fist::style::Text const
          style = view::title::style;
        }

        namespace body
        {
          static
          fist::style::Text const
          style = view::body::style;
        }
      }
    }

    namespace link_ready
    {
      namespace view
      {
        namespace body
        {
          static
          fist::style::Text const
          style(fist::Font(DefaultFont, 12), QColor(0x13, 0x13, 0x13));

          QString const
          text("A link to your file has been copied to your clipboard!");
        }
      }
    }
  }
}

#endif
