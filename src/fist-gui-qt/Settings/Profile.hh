#ifndef FIST_GUI_QT_SETTINGS_PROFILE_HH
# define FIST_GUI_QT_SETTINGS_PROFILE_HH

# include <memory>

# include <QWidget>
# include <QLabel>
# include <QLineEdit>
# include <QDialog>
# include <QPushButton>

# include <elle/attribute.hh>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace prefs
  {
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
  }
}

#endif
