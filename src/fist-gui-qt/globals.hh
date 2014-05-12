#ifndef GLOBALS_HH
# define GLOBALS_HH

# include <QFont>
# include <QColor>
# include <QLabel>

// This file should contain every single dimension, font, color, hint to avoid
// per file constants.

// XXX: Rework the structure.
// A good structure could be, but it's really verbose.
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

namespace
{
  // TextStyle provides a friendly api to set Widget font and color.
  struct TextStyle
  {
    TextStyle(QFont const& font,
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
    QFont _font;
    QColor _color;
    Qt::Alignment _alignement;
  };
}

namespace regexp
{
  static QString email = "[a-z0-9\\._%+-]+@[a-z0-9\\.-]+\\.[a-z]{2,12}";
}

namespace view
{
  static
  QColor const
  background(0xF8, 0xF8, 0xF8);

  namespace tooltip
  {
    static
    TextStyle const
    style(QFont("Arial", 11), QColor(0xF8, 0xF8, 0xF8), Qt::AlignVCenter);
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
      TextStyle const
      style(QFont("Arial", 11), QColor(0x33, 0x33, 0x33));

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
      TextStyle const
      style(QFont("Arial", 11), QColor(0x33, 0x33, 0x33));

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
      TextStyle const
      style(QFont("Arial", 11), QColor(0xEE, 0x11, 0x11), Qt::AlignCenter);
    }

    namespace links
    {
      static
      TextStyle const
      style(QFont("Arial", 9), QColor(0x3D, 0x91, 0xD4));

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
             "href=\"https://infinit.io/forgot_password\">"
             "forgot password?</a>");
      }

      namespace need_an_account
      {
        static
        QString const
        text("<a style=\"text-decoration: none; color: #489FCE;\" "
             "href=\"https://infinit.io/register\">"
             "need an account?</a>");
      }
    }

    namespace version
    {
      static
      TextStyle const
      style(QFont("Arial", 8.5), QColor(0x88, 0x88, 0x88));
    }

    namespace footer
    {
      static
      TextStyle const
      style(QFont("Arial", 11, QFont::Bold), Qt::white);

      static
      QString const
      text("LOGIN");
    }
  }

  namespace transaction
  {
    namespace no_notification
    {
      static
      TextStyle const
      style(QFont("Arial", 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);
    }

    static
    QMargins const
    margins(13, 13, 13, 13);

    namespace peer
    {
      static
      TextStyle const
      style(QFont("Arial", 10), QColor(0x25, 0x25, 0x25));
    }

    namespace files
    {
      static
      TextStyle const
      style(QFont("Arial", 9.5), QColor(0xA8, 0xA8, 0xA8));
    }

    namespace date
    {
      static
      TextStyle const
      style(QFont("Arial", 9.5), QColor(0xA8, 0xA8, 0xA8));
    }
  }

  namespace send
  {
    namespace file
    {
      namespace name
      {
        static
        TextStyle const
        style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
      }

      namespace size
      {
        static
        TextStyle const
        style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);
      }

      static
      QColor const
      background(0xF8, 0xF8, 0xF8);
    }

    namespace file_adder
    {
      static
      TextStyle const
      style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3), Qt::AlignVCenter);

      static
      TextStyle const
      hover_style(QFont("Arial", 9), QColor(0x3C, 0x91, 0xD4), Qt::AlignVCenter);

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
      TextStyle const
      style(QFont("Arial", 9), Qt::AlignVCenter);

      static
      QString const
      text("Search for a friend...");
    }

    namespace user
    {
      namespace fullname
      {
        static
        TextStyle const
        style(QFont("Arial", 9), QColor(0x25, 0x25, 0x25));
      }

      namespace handle
      {
        static
        TextStyle const
        style(QFont("Arial", 8), QColor(0xC4, 0xC4, 0xC4));
      }
    }

  }
}

#endif
