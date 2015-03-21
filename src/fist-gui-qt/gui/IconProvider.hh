#ifndef FIST_GUI_QT_GUI_ICONPROVIDER_HH
# define FIST_GUI_QT_GUI_ICONPROVIDER_HH

# include <unordered_map>
# include <map>

# include <QPixmap>
# include <QString>

# include <elle/attribute.hh>

namespace fist
{
  namespace gui
  {
    // XXX: Use the file mimetype.
    class IconProvider
    {
    public:
      IconProvider();

    public:
      virtual
      ~IconProvider() = default;

    public:
      enum class Type{
        archive,
        cat,
        folder,
        illustrator,
        music,
        photoshop,
        image,
        powerpoint,
        text,
        unknown,
        video,
        document,
        windows,
        linux,
        macosx,
        ios,
        android,
        unknown_os,
      };

    public:
      QPixmap const&
      icon(QString const& filename);

      void
      add_icon(QString const& extension,
               QPixmap const& pixmap);


      typedef std::map<Type, QPixmap> TypeIcon;
      typedef std::unordered_map<std::string, Type> TypeTranslator;
      typedef std::unordered_map<std::string, QPixmap> UserDefinedIcons;
      ELLE_ATTRIBUTE_R(TypeIcon, icons);
      ELLE_ATTRIBUTE(TypeTranslator, translator);
      ELLE_ATTRIBUTE(UserDefinedIcons, user_icons);
    };

    IconProvider&
    icon_provider();
  }
}

#endif
