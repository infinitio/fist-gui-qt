#include <fist-gui-qt/Settings.hh>

namespace fist
{
  Settings&
  settings()
  {
    static Settings settings("Infinit.io", "Infinit");
    return settings;
  }

  /*-----------.
  | GroupProxy |
  `-----------*/
  Settings::Group::GroupProxy::GroupProxy(Settings& master,
                                          QString const& name)
    : _master(master)
  {
    this->_master._settings.beginGroup(name);
  }

  Settings::Group::GroupProxy::~GroupProxy()
  {
    this->_master._settings.endGroup();
  }

  QSettings&
  Settings::Group::GroupProxy::settings()
  {
    return this->_master._settings;
  }

  QSettings const&
  Settings::Group::GroupProxy::settings() const
  {
    return this->_master._settings;
  }

  /*------.
  | Group |
  `------*/
  Settings::Group::Group(Settings& master,
                         QString const& name)
    : _master(master)
    , _name(name)
  {}

  QVariant
  Settings::Group::get(QString const& key,
                       QVariant const& default_value) const
  {
    GroupProxy proxy(this->_master, this->_name);
    return proxy.settings().value(key, default_value);
  }

  void
  Settings::Group::set(QString const& key,
                       QString const& value)
  {
    GroupProxy proxy(this->_master, this->_name);
    proxy.settings().setValue(key, QString(value));
  }

  bool
  Settings::Group::exists(QString const& key) const
  {
    GroupProxy proxy(this->_master, this->_name);
    return proxy.settings().contains(key);
  }

  /*----------.
  | Settrings |
  `----------*/
  Settings::Settings(QString const& company_name,
                     QString const& product_name)
    : _settings(company_name, product_name)
  {}

  Settings::Group&
  Settings::operator [] (QString const& group_name)
  {
    auto name = group_name.toStdString();
    if (this->_groups.find(name) == this->_groups.end())
      this->_groups.emplace(name, Group(*this, group_name));
    return this->_groups.at(name);
  }

  /*----------.
  | Printable |
  `----------*/
  void
  Settings::print(std::ostream& stream) const
  {
    stream << "settings";
  }
}
