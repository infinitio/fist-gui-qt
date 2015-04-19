#include <fist-gui-qt/model/Device.hh>
#include <fist-gui-qt/gui/IconProvider.hh>
#include <fist-gui-qt/regexp.hh>
#include <fist-gui-qt/utils.hh>

namespace fist
{
  namespace model
  {
    static
    gui::IconProvider::Type
    os_to_type(QString const& name)
    {
      if (name == "Android")
        return gui::IconProvider::Type::android;
      else if (name == "iOS")
        return gui::IconProvider::Type::ios;
      else if (name == "MacOSX")
        return gui::IconProvider::Type::macosx;
      else if (name == "Windows")
        return gui::IconProvider::Type::windows;
      else if (name == "Linux")
        return gui::IconProvider::Type::unknown_os;
      return gui::IconProvider::Type::unknown_os;
    }

    static
    QString
    os_to_pretty_name(QString const& name)
    {
      if (name == "Android")
        return "Android";
      else if (name == "iOS")
        return "iPhone";
      else if (name == "MacOSX")
        return "Mac";
      else if (name == "Windows")
        return "PC";
      return "PC";
    }

    Device::Device(surface::gap::Device const& device)
      : _id(QString::fromStdString(device.id.repr()))
      , _name(QString::fromStdString(device.name))
      , _os(device.os ? QString::fromStdString(device.os.get()) : QString{})
      , _type(os_to_type(this->_os))
    {
      // Remove when devices will be named properly.
      if (fist::regexp::uuid::checker.exactMatch (this->_name))
        this->_name = "My " + os_to_pretty_name(this->_os);
    }

    void
    Device::print(std::ostream& stream) const
    {
      stream << "Device(" << this->_name << ", " << this->_os << ")";
    }
  }
}
