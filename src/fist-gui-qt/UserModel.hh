#ifndef USERMODEL_HH
# define USERMODEL_HH

# include <surface/gap/gap.hh>

# include <elle/Printable.hh>

#include <surface/gap/fwd.hh>

# include <QString>
# include <QPixmap>
# include <QVariant>

# include <memory>

class UserModel:
  public elle::Printable
{
public:
  UserModel(gap_State* state,
            uint32_t id);

  UserModel(UserModel const&) = default;

  QString const&
  fullname() const;

  QString const&
  handle() const;

  uint32_t
  id() const;

  bool
  user_status();

  QPixmap const&
  avatar() const;

  bool
  new_avatar() const;

  void
  avatar_available();

private:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _fullname;
  mutable QString _handle;
  mutable QPixmap _avatar;
  mutable bool _default_avatar;
  mutable bool _new_avatar;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

};

#endif
