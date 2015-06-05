#ifndef FIST_GUI_QT_SETTINGS_WINDOW_HH
# define FIST_GUI_QT_SETTINGS_WINDOW_HH

# include <iostream>

# include <QDialog>
# include <QTabWidget>
# include <QLabel>
# include <QLineEdit>
# include <QCheckBox>
# include <QTabBar>

# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/AvatarIcon.hh>

# include <elle/attribute.hh>

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
    };

    class ChangePassword
      : public QDialog
    {
    public:
      ChangePassword(QWidget* parent);

      ELLE_ATTRIBUTE_RW(QLabel*, message);
      ELLE_ATTRIBUTE_RW(QLineEdit*, current_password);
      ELLE_ATTRIBUTE_RW(QLineEdit*, password);
      ELLE_ATTRIBUTE_RW(QPushButton*, validate);
    };

    class Profile
      : public QWidget
    {
      Q_OBJECT

    public:
      typedef QWidget Super;

    public:
      explicit
      Profile(fist::State& state,
                QWidget *parent = nullptr);

      ELLE_ATTRIBUTE(fist::State&, state);

    private slots:

      void
      _update_fullname();

      void
      _update_handle();

      void
      _change_avatar();

      void
      _change_password();

      void
      _change_password2();

      void
      _on_avatar_updated();

    private:
      ELLE_ATTRIBUTE(AvatarIcon*, avatar);
      ELLE_ATTRIBUTE(QLineEdit*, fullname);
      ELLE_ATTRIBUTE(QLineEdit*, handle);
      ELLE_ATTRIBUTE(std::unique_ptr<ChangePassword>, password_window);
    };


    class TabBar
      : public QTabBar
    {
    public:
      TabBar(QWidget* parent);

    private:
      void
      paintEvent(QPaintEvent* event) override;
    };

    class TabWidget:
      public QTabWidget
    {
    public:
      TabWidget(QWidget* parent)
        : QTabWidget(parent)
      {
        this->setTabBar(new TabBar(this));
      }

    private:
      void
      paintEvent(QPaintEvent* event) override;
    };

    class Window:
      public QDialog
    {
      Q_OBJECT;

    public:
      explicit
      Window(fist::State& state,
             QWidget* parent = nullptr);

    private:
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(TabWidget*, tabWidget);
    };
  }
}

#endif
