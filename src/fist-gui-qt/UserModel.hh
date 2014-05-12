#ifndef FIST_GUI_QT_USERMODEL_HH
# define FIST_GUI_QT_USERMODEL_HH

# include <memory>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/gap.hh>
# include <surface/gap/fwd.hh>

# include <QString>
# include <QPixmap>
# include <QVariant>

# include <fist-gui-qt/TransactionModel.hh>

class UserModel:
  public QObject,
  public elle::Printable
{
  typedef std::unordered_set<std::unique_ptr<TransactionModel>> Transactions;

public:
  UserModel(gap_State* state,
            uint32_t id);

  UserModel(UserModel const&) = default;

  QString const&
  fullname() const;

  QString const&
  handle() const;

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
  gap_State* _state;
  ELLE_ATTRIBUTE_R(uint32_t, id);

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  ELLE_ATTRIBUTE_P(QString, fullname, mutable);
  ELLE_ATTRIBUTE_P(QString, handle, mutable);
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
};

# include <fist-gui-qt/UserModel.hxx>

#endif
