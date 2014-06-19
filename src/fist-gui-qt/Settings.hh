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
  // Wrap the Qt settings class to avoid patters like:
  // - BeginGroup(...)
  // - Get or Modify the group
  // - EndGroup()
  // Group are accessible via subscription operator ([]) via their name,
  // returning a 'proxy' in which you can:
  // - get a setting value.
  // - set a setting value.
  // - check if setting exists.
  //
  // XXX: Handle subgroup.
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
        // Open the group.
        GroupProxy(Settings& master,
                   QString const& name);

        // Close the group.
        virtual
        ~GroupProxy();

        // Return the editable setting.
        QSettings&
        settings();

        // Return the non-editable setting.
        QSettings const&
        settings() const;

        ELLE_ATTRIBUTE(Settings&, master);
      };

    private:
      Group(Settings& master,
            QString const& name);

    public:
      // Return the setting value or the default value passed.
      QVariant
      get(QString const& key,
          QVariant const& default_value = QVariant()) const;

      // Set the setting value.
      void
      set(QString const& key,
          QVariant const& value);

      // Remove a setting.
      void
      remove(QString const& key);

      // Check if the setting exists.
      bool
      exists(QString const& key) const;

      ELLE_ATTRIBUTE(Settings&, master);
      ELLE_ATTRIBUTE(QString, name);

      friend Settings;
    };

    Settings(QString const& company_name,
             QString const& product_name);

    Group&
    operator [] (QString const& group_name);

    typedef std::unordered_map<std::string, Group> Groups;
    ELLE_ATTRIBUTE_P(Groups, groups, mutable);
    ELLE_ATTRIBUTE(QSettings, settings);

    friend Group::GroupProxy;

    /*----------.
    | Printable |
    `----------*/
    void
    print(std::ostream& stream) const override;
  };

  Settings&
  settings();
}
#endif
