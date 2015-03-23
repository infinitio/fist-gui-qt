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
static QString DefaultFont(Arial);

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
  class Font
  {
  public:
    Font(QString const& family,
         int pointSize,
         int weight = -1):
      inner(family, pointSize, weight)
    {
      inner.setPixelSize(pointSize * 1.4);
    }
    operator QFont& () { return inner; }
    operator QFont const& () const { return inner; }
    QFont inner;
  };

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
  version_style(fist::Font(DefaultFont, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);

  static
  fist::style::Text const
  download_folder_title_style(fist::Font(DefaultFont, 10), QColor(0x25, 0x25, 0x25), Qt::AlignLeft);

  static
  fist::style::Text const
  download_folder_style(fist::Font(DefaultFont, 9), QColor(0x33, 0x33, 0x33), Qt::AlignLeft);

  namespace links
  {
    namespace no_links
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);
    }

    namespace file
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 9.5), QColor(0x25, 0x25, 0x25));

      static
      fist::style::Text const
      failed_style(fist::Font(DefaultFont, 9.5), QColor(0xD3, 0x15, 0x15));
    }

    namespace status
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 8.5), QColor(0xA8, 0xA8, 0xA8));
    }

    namespace counter
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 9), QColor(0xFF, 0xFF, 0xFF), Qt::AlignCenter);

    }
  }

  namespace tab
  {
    static
    fist::style::Text const
    style(fist::Font(Montserrat, 9), QColor(0x8B, 0x8B, 0x83));

    static
    QColor
    bar_color(0xE6, 0xE6, 0xE6);

    static
    fist::style::Text const
    hover_style(fist::Font(Montserrat, 9), QColor(0x51, 0x51, 0x49));

    static
    QColor
    bar_hover_color(0xD5, 0xD5, 0xD5);

    static
    fist::style::Text const
    selected_style(fist::Font(Montserrat, 9), QColor(0x2B, 0xBE, 0xBD));

    namespace counter
    {
      static
      fist::style::Text const
      style(fist::Font(Montserrat, 7), tab::style.color(), Qt::AlignTop);

      static
      fist::style::Text const
      hover_style(fist::Font(Montserrat, 7), tab::hover_style.color(), Qt::AlignTop);

      static
      fist::style::Text const
      selected_style(fist::Font(Montserrat, 7), tab::selected_style.color(), Qt::AlignTop);
    }

  }

  namespace tooltip
  {
    static
    fist::style::Text const
    style(fist::Font(DefaultFont, 11), QColor(0xF8, 0xF8, 0xF8), Qt::AlignVCenter);
  }

  namespace transaction
  {
    namespace no_notification
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 10), QColor(0x25, 0x25, 0x25), Qt::AlignCenter);
    }

    static
    QMargins const
    margins(13, 13, 13, 13);

    namespace peer
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 10), QColor(0x25, 0x25, 0x25));
    }

    namespace files
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 9.5), QColor(0xA8, 0xA8, 0xA8));

      static
      fist::style::Text const
      hover_style(fist::Font(DefaultFont, 9), QColor(0x3D, 0x91, 0xD4));
    }

    namespace date
    {
      static
      fist::style::Text const
      style(fist::Font(DefaultFont, 8), QColor(0xA8, 0xA8, 0xA8));
    }
  }
}

#endif
