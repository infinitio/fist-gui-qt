#ifndef USERMODEL_HH
# define USERMODEL_HH

# include <memory>
# include <unordered_set>

# include <QPixmap>
# include <QString>
# include <QVariant>

# include <elle/Printable.hh>

# include <surface/gap/gap.h>

# include <fist-gui-qt/TransactionModel.hh>

class UserModel:
  public elle::Printable
{
public:
  typedef std::unordered_set<TransactionModel> Transactions;

public:
  UserModel(gap_State* state,
            uint32_t id);

  UserModel(UserModel const&) = default;
  ~UserModel();

  virtual
  uint32_t
  id() const;

  virtual
  QString const&
  fullname() const;

  virtual
  QString const&
  handle() const;

  virtual
  Transactions const&
  transactions() const;

  virtual
  bool
  status() const;

  virtual
  QPixmap const&
  avatar() const;

  bool
  new_avatar() const;

  void
  avatar_available();

protected:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _fullname;
  mutable QString _handle;
  mutable Transactions _transactions;
  mutable QPixmap _avatar;
  mutable bool _default_avatar;
  mutable bool _new_avatar;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

};

# include <fist-gui-qt/UserModel.hxx>

#endif
