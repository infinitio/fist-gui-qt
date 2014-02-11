#ifndef GLOBALS_HH
# define GLOBALS_HH

# include <QFont>
# include <QColor>

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
              QColor const& color = QColor(0x25, 0x25, 0x25)):
      _font(font),
      _color(color)
    {}

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
  };
}

namespace regexp
{
  static QString email = "[a-z0-9\\._%+-]+@[a-z0-9\\.-]+\\.[a-z]{2,4}";
}

namespace view
{
  static
  QColor const
  background(0xF8, 0xF8, 0xF8);

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
      size(240, 37);
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
      size(240, 37);
    }

    namespace message
    {
      static
      TextStyle const
      style(QFont("Arial", 11), QColor(0xEE, 0x11, 0x11));
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
        text("<a style=\"text-decoration: none;\" href=\"https://infinit.io/forgot_password\">forgot pasword?</a>");
      }

      namespace need_an_account
      {
        static
        QString const
        text("<a style=\"text-decoration: none;\" href=\"https://infinit.io/register\">need an account?</a>");
      }
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
      style(QFont("Arial", 10), QColor(0x25, 0x25, 0x25));
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
  }

  namespace send
  {
    namespace file
    {
      namespace name
      {
        static
        TextStyle const
        style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3));
      }

      namespace size
      {
        static
        TextStyle const
        style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3));
      }

      static
      QColor const
      background(0xF8, 0xF8, 0xF8);
    }

    namespace file_adder
    {
      static
      TextStyle const
      style(QFont("Arial", 9), QColor(0xB3, 0xB3, 0xB3));

      static
      QColor const
      background(0xF8, 0xF8, 0xF8);

      static
      QString const
      text("Add files...");
    }

    namespace search_field
    {
      static
      TextStyle const
      style(QFont("Arial", 9, QFont::Bold));

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
        style(QFont("Arial", 9), QColor(0x00, 0x00, 0x00));
      }

      namespace handle
      {
        static
        TextStyle const
        style(QFont("Arial", 7.5), QColor(0xC4, 0xC4, 0xC4));
      }
    }

  }
}

#endif
