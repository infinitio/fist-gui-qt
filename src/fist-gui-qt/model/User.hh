#ifndef FIST_GUI_QT_MODEL_USER_HH
# define FIST_GUI_QT_MODEL_USER_HH

# include <memory>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <QString>
# include <QPixmap>
# include <QVariant>

# include <fist-gui-qt/model/Model.hh>
# include <fist-gui-qt/model/Device.hh>
# include <fist-gui-qt/fwd.hh>

# include <surface/gap/User.hh>

namespace fist
{
  namespace model
  {
    // The User model is a re-objectification of the User.
    // Inheriting from Model make User an QObject, allowing signals to be sent.
    // Friendship with State allows State to trigger thoose signals to control
    // the widget linked to the model.
    class User
      : public Model
    {
      typedef Model Super;
    public:
      User(fist::State& state,
           uint32_t id);
      User(fist::State& state,
           surface::gap::User const& user);
      virtual
      ~User() = default;

      User(User const&) = default;

      /// Return true if it's the current user.
      bool
      me() const;

      QString
      fullname() const;

      QString
      handle() const;

      void
      deleted(bool status);

      bool
      deleted();

      void
      status(bool status);

      bool
      status() const;

      bool
      swagger() const;

      bool
      ghost() const;

      void
      swagger(bool);

      QPixmap const&
      avatar() const;

      bool
      new_avatar() const;

      void
      avatar_available();

    private:
      // Every attributes are marked as mutable in order to allow lazy
      // evaluation.
      ELLE_ATTRIBUTE_Rw(surface::gap::User, model);
      ELLE_ATTRIBUTE_P(QPixmap, avatar, mutable);
      ELLE_ATTRIBUTE_P(bool, default_avatar, mutable);
      ELLE_ATTRIBUTE_P(bool, new_avatar, mutable);
      typedef std::vector<model::Device> Devices;
      ELLE_ATTRIBUTE_RX(Devices, devices);

    signals:
      void
      avatar_updated() const;

      void
      status_updated();

    private:
      /*----------.
        | Printable |
        `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      Q_OBJECT;

    private:
      friend fist::State;
    };
  }
}

# include <fist-gui-qt/model/User.hxx>

#endif
