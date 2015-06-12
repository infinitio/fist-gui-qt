#ifndef FIST_GUI_QT_SETTINGS_GENERAL_HH
# define FIST_GUI_QT_SETTINGS_GENERAL_HH

# include <QWidget>
# include <QLabel>
# include <QCheckBox>
# include <QLineEdit>

# include <elle/attribute.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace prefs
  {
    class General
      : public QWidget
    {
      Q_OBJECT;

    public:
      typedef QWidget Super;
    public:
      explicit
      General(fist::State& state,
              QWidget *parent = nullptr);

   private slots:
      void
      _modify_startup_option(int);

      void
      _choose_download_folder();

      void
      _update_device_name();

      void
      _modify_reminders(int);

    private:
      void
      _set_download_folder(QString const&);

    private:
      void
      showEvent(QShowEvent* event) override;
    private:
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(QLabel*, download_folder);
      ELLE_ATTRIBUTE(QCheckBox*, launch_at_startup);
      ELLE_ATTRIBUTE(QLineEdit*, device_name);
      ELLE_ATTRIBUTE(QCheckBox*, reminders);
    };
  }
}

#endif
