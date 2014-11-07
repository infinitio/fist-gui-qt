#ifndef FIST_GUI_QT_GLOBALS_HH
# define FIST_GUI_QT_GLOBALS_HH

# include <QColor>
# include <QFont>
# include <QLabel>
# include <QVector>

# include <elle/attribute.hh>

static QString Arial("Arial");
static QString Lucida("LucidaGrande");
static QString Montserrat("Montserrat");
static QString Font(Arial);

// This file should contain every single dimension, font, color, hint to avoid
// per file constants.

// XXX: Rework the structure.
// A good structure could be (but it's really verbose).
// view
// - login
//   - email
//     - width
//     - height
//     - style
//   - password
//     - width
//     - height
//     - style
// - users
//   - list
//     - item
//       - width
//       - height
// ...
// Otherwise, qss should work too.

namespace fist
{
  namespace style
  {
    // TextStyle provides a friendly api to set Widget font and color.
    struct Text
    {
      Text(QFont const& font,
           QColor const& color = QColor(0x25, 0x25, 0x25),
           Qt::Alignment const& aligment = Qt::AlignLeft | Qt::AlignVCenter):
        _font(font),
        _color(color),
        _alignement(aligment)
      {
        this->_font.setStyleStrategy(QFont::PreferAntialias);
      }

      void
      operator ()(QLabel& target) const
      {
        target.setAlignment(this->_alignement);
        this->operator ()(*(static_cast<QWidget*>(&target)));
      }

      void
      operator ()(QWidget& target) const
      {
        target.setFont(this->_font);
        QPalette palette = target.palette();
        {
          palette.setColor(QPalette::WindowText, this->_color);
        }
        target.setPalette(palette);
      }

    private:
      ELLE_ATTRIBUTE_RX(QFont, font);
      ELLE_ATTRIBUTE_RW(QColor, color);
      ELLE_ATTRIBUTE_R(Qt::Alignment, alignement);
    };
  }
}

namespace regexp
{
  static QString email = "[a-z0-9\\._%+-]+@[a-z0-9\\.-]+\\.[a-z]{2,12}";
}

namespace list
{
  namespace separator
  {
    static
    QVector<QColor> const
    colors({QColor(0xE3, 0xE3, 0xE3), Qt::white});
  }
}

namespace view
{
  static
  QColor const
  background(0xF8, 0xF8, 0xF8);

  static
  fist::style::Text const
  version_style(QFont(Font, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);

  namespace tab
  {
    static
    fist::style::Text const
    style(QFont(Montserrat, 10), QColor(0x8B, 0x8B, 0x83));

    static
    QColor
    bar_color(0xE6, 0xE6, 0xE6);

    static
    fist::style::Text const
    hover_style(QFont(Montserrat, 10), QColor(0x51, 0x51, 0x49));

    static
    QColor
    bar_hover_color(0xD5, 0xD5, 0xD5);

    static
    fist::style::Text const
    selected_style(QFont(Montserrat, 10), QColor(0x2B, 0xBE, 0xBD));

    namespace counter
    {
      static
      fist::style::Text const
      style(QFont(Montserrat, 7), tab::style.color(), Qt::AlignTop);

      static
      fist::style::Text const
      hover_style(QFont(Montserrat, 7), tab::hover_style.color(), Qt::AlignTop);

      static
      fist::style::Text const
      selected_style(QFont(Montserrat, 7), tab::selected_style.color(), Qt::AlignTop);
    }

  }

  namespace tooltip
  {
    static
    fist::style::Text const
    style(QFont(Font, 11), QColor(0xF8, 0xF8, 0xF8), Qt::AlignVCenter);
  }

  namespace login
  {
    static
    QSize const
    size(320, 400);

    static
    Qt::WindowFlags const
    flags = Qt::FramelessWindowHint;

    namespace email
    {
      static
      fist::style::Text const
      style(QFont(Font, 11), QColor(0x33, 0x33, 0x33));

      static
      QString const
      placeholder("Email");

      static
      QSize const
      size(240, 38);
    }

    namespace password
    {
      static
      fist::style::Text const
      style(QFont(Font, 11), QColor(0x33, 0x33, 0x33));

      static
      QString const
      placeholder("Password");

      static
      QSize const
      size(240, 38);
    }

    namespace message
    {
      static
      fist::style::Text const
      error_style(QFont(Font, 10.5), QColor(0xEE, 0x11, 0x11), Qt::AlignCenter);

      static
      fist::style::Text const
      warning_style(QFont(Font, 10.5), QColor(0x88, 0x88, 0x88), Qt::AlignCenter);
    }

    namespace links
    {
      static
      fist::style::Text const
      style(QFont(Font, 9), QColor(0x3D, 0x91, 0xD4));

      static
      Qt::TextInteractionFlags const
      interration_flags(
        Qt::TextSelectableByMouse |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard);

      namespace forgot_password
      {
        static
        QString const
        text("<a style=\"text-decoration: none; color: #489FCE;\" "
             "href=\"https://infinit.io/forgot_password?utm_source=app&utm_medium=windows\">"
             "forgot password?</a>");
      }

      namespace need_an_account
      {
        static
        QString const
        text("<a style=\"text-decoration: none; color: #489FCE;\" "
             "href=\"https://infinit.io/register?utm_source=app&utm_medium=windows\">"
             "need an account?</a>");
      }
    }

    namespace version
    {
      static
      fist::style::Text const
      style(QFont(Font, 8.5), QColor(0x88, 0x88, 0x88));
    }

    namespace footer
    {
      static
      fist::style::Text const
      style(QFont(Font, 11, QFont::Bold), Qt::white);

      static
      QString const
      text("LOGIN");
    }
  }

  namespace links
  {
    namespace no_links
    {
      static
      fist::style::Text const
      style(QFont(Font, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);
    }

    namespace file
    {
      static
      fist::style::Text const
      style(QFont(Font, 9.5), QColor(0x25, 0x25, 0x25));

      static
      fist::style::Text const
      failed_style(QFont(Font, 9.5), QColor(0xD3, 0x15, 0x15));
    }

    namespace status
    {
      static
      fist::style::Text const
      style(QFont(Font, 8.5), QColor(0xA8, 0xA8, 0xA8));
    }

    namespace counter
    {
      static
      fist::style::Text const
      style(QFont(Font, 9), QColor(0xFF, 0xFF, 0xFF), Qt::AlignCenter);

    }
  }

  namespace transaction
  {
    namespace no_notification
    {
      static
      fist::style::Text const
      style(QFont(Font, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);
    }

    static
    QMargins const
    margins(13, 13, 13, 13);

    namespace peer
    {
      static
      fist::style::Text const
      style(QFont(Font, 10), QColor(0x25, 0x25, 0x25));
    }

    namespace files
    {
      static
      fist::style::Text const
      style(QFont(Font, 9.5), QColor(0xA8, 0xA8, 0xA8));

      static
      fist::style::Text const
      hover_style(QFont(Font, 9), QColor(0x3D, 0x91, 0xD4));
    }

    namespace date
    {
      static
      fist::style::Text const
      style(QFont(Font, 8), QColor(0xA8, 0xA8, 0xA8));
    }
  }

  namespace send
  {
    namespace file
    {
      namespace name
      {
        static
        fist::style::Text const
        style(QFont(Font, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
      }

      namespace size
      {
        static
        fist::style::Text const
        style(QFont(Font, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
      }

      static
      QColor const
      background(0xF8, 0xF8, 0xF8);
    }

    namespace file_adder
    {
      static
      fist::style::Text const
      style(QFont(Font, 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);

      static
      fist::style::Text const
      hover_style(QFont(Font, 9), QColor(0x3C, 0x91, 0xD4), Qt::AlignVCenter);

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
      style(QFont(Font, 9), QColor(0x25, 0x25, 0x25), Qt::AlignVCenter);

      static
      QString const
      text("Search or select a user...");
    }

    namespace message
    {
      static
      fist::style::Text const
      style(QFont(Font, 9), QColor(0x25, 0x25, 0x25), Qt::AlignVCenter);

      static
      QString const
      text("Optional message...");

      namespace remaining_characters
      {
        static
        fist::style::Text const
        style(QFont(Font, 7), QColor(0xA5, 0xA5, 0xA5), Qt::AlignVCenter);

      }
    }

    namespace user
    {
      namespace fullname
      {
        static
        fist::style::Text const
        style(QFont(Font, 9), QColor(0x25, 0x25, 0x25));
      }

      namespace handle
      {
        static
        fist::style::Text const
        style(QFont(Font, 8), QColor(0xC4, 0xC4, 0xC4));
      }
    }

  }
}

#endif
