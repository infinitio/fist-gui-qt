#include <memory>

#include <QPixmap>
#include <QStringList>

#include <fist-gui-qt/gui/IconProvider.hh>
#include <fist-gui-qt/gui/IconProvider/known.hh>

namespace fist
{
  namespace gui
  {
    namespace
    {
      static
      std::unique_ptr<IconProvider> global_icon_provider;
    }

    IconProvider&
    icon_provider()
    {
      if (global_icon_provider == nullptr)
      {
        global_icon_provider.reset(new IconProvider);
      }

      return *global_icon_provider;
    }

    IconProvider::IconProvider()
      : _icons(known_icons())
      , _translator(known_types)
      , _user_icons()
    {}

    QPixmap const&
    IconProvider::icon(QString const& filename)
    {
      QStringList parts = filename.split(".");
      auto extension = parts.isEmpty()
        ? std::string()
        : parts.last().toStdString();

      if (this->_user_icons.find(extension) != this->_user_icons.end())
        return this->_user_icons.at(extension);

      if (this->_translator.find(extension) == this->_translator.end())
      {
        return this->_icons.at(Type::unknown);
      }
      return this->_icons.at(this->_translator.at(extension));
    }

    void
    IconProvider::add_icon(QString const& extension,
                           QPixmap const& pixmap)
    {
      this->_user_icons[extension.toStdString()] = pixmap;
    }

  }
}
