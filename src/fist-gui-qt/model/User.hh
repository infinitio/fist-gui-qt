#ifndef FIST_GUI_QT_MODEL_USER_HH
# define FIST_GUI_QT_MODEL_USER_HH

# include <memory>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <QString>
# include <QPixmap>
# include <QVariant>

# include <fist-gui-qt/model/Transaction.hh>
# include <fist-gui-qt/model/Model.hh>
# include <fist-gui-qt/fwd.hh>

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
      typedef std::unordered_set<std::unique_ptr<Transaction>> Transactions;

    public:
      User(fist::State& state,
                uint32_t id);
      virtual
      ~User() = default;

      User(User const&) = default;

      QString const&
      fullname() const;

      QString const&
      handle() const;

      bool
      deleted();

      bool
      user_status();

      QPixmap const&
      avatar() const;

      Transactions const&
      transactions() const;

      bool
      new_avatar() const;

      void
      avatar_available();

    private:
      // Every attributes are marked as mutable in order to allow lazy
      // evaluation.
      ELLE_ATTRIBUTE_P(QString, fullname, mutable);
      ELLE_ATTRIBUTE_P(QString, handle, mutable);
      ELLE_ATTRIBUTE_RW(bool, deleted);
      ELLE_ATTRIBUTE_P(QPixmap, avatar, mutable);
      ELLE_ATTRIBUTE_P(Transactions, transactions, mutable);
      ELLE_ATTRIBUTE_P(bool, default_avatar, mutable);
      ELLE_ATTRIBUTE_P(bool, new_avatar, mutable);

    signals:
      void
      avatar_updated() const;

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
