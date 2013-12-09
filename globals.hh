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
              QColor const& color = QColor(0x00, 0x00, 0x00)):
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

namespace view
{
  namespace login
  {
    static
    QSize const
    size(320, 400);

    static
    Qt::WindowFlags const
    flags = Qt::FramelessWindowHint | Qt::Tool;

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
      size(240, 40);
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
      size(240, 40);
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
      style(QFont("Arial", 9), QColor(0x30, 0x30, 0x00));

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
        text("<a href=\"http://infinit.io/forgot_password\">forgot pasword?</a>");
      }

      namespace need_an_account
      {
        static
        QString const
        text("<a href=\"http://infinit.io/register\">need an account?</a>");
      }
    }

    namespace footer
    {
      static
      TextStyle const
      style(QFont("Arial", 12, QFont::Bold), QColor(0xFF, 0xFF, 0xFF));

      static
      QString const
      text("Login");
    }
  }
}

#endif
