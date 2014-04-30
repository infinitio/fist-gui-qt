#ifndef FIST_GUI_QT_SETTINGS_HH
# define FIST_GUI_QT_SETTINGS_HH

# include <string>
# include <unordered_map>

# include <QSettings>
# include <QVariant>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

namespace fist
{
  class Settings:
    public elle::Printable
  {
  public:
    class Group
    {
    private:
      class GroupProxy
      {
      public:
        GroupProxy(Settings& master,
                   QString const& name)
          : _master(master)
        {
          this->_master._settings.beginGroup(QString(name));
        }

        ~GroupProxy()
        {
          this->_master._settings.endGroup();
        }

        QSettings&
        settings()
        {
          return this->_master._settings;
        }

        QSettings const&
        settings() const
        {
          return this->_master._settings;
        }

        ELLE_ATTRIBUTE(Settings&, master);
      };

    private:
      Group(Settings& master,
            QString const& name)
        : _master(master)
        , _name(name)
      {}

    public:
      QVariant
      get(QString const& key,
          QVariant const& default_value = QVariant()) const
      {
        GroupProxy proxy(this->_master, this->_name);
        return proxy.settings().value(key, default_value);
      }

      void
      set(QString const& key,
          QString const& value)
      {
        GroupProxy proxy(this->_master, this->_name);
        proxy.settings().setValue(key, QString(value));
      }

      bool
      exists(QString const& key) const
      {
        GroupProxy proxy(this->_master, this->_name);
        return proxy.settings().contains(key);
      }

      ELLE_ATTRIBUTE(Settings&, master);
      ELLE_ATTRIBUTE(QString, name);

      friend Settings;
    };

    Settings()
      : _settings("Infinit.io", "Infinit")
    {}

    Group&
    operator [] (QString const& group_name)
    {
      auto name = group_name.toStdString();
      if (this->_groups.find(name) == this->_groups.end())
        this->_groups.emplace(name, Group(*this, group_name));
      return this->_groups.at(name);
    }

    typedef std::unordered_map<std::string, Group> Groups;
    ELLE_ATTRIBUTE_P(Groups, groups, mutable);
    ELLE_ATTRIBUTE(QSettings, settings);

    friend Group::GroupProxy;

    /*----------.
    | Printable |
    `----------*/
    void
    print(std::ostream& stream) const override
    {
      stream << "settings";
    }

  };

  Settings&
  settings();
}
#endif
