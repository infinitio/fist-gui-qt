#ifndef USERMODEL_HH
# define USERMODEL_HH

# include <surface/gap/gap.h>

# include <QString>
# include <QPixmap>
# include <QVariant>

# include <memory>

class UserModel:
  public QVariant
{
public:
  UserModel(gap_State* state,
            uint32_t id);

  UserModel(UserModel const&) = default;

  QString const&
  fullname() const;

  uint32_t
  id() const;

  QPixmap const&
  avatar() const;

  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _fullname;
  mutable QPixmap _avatar;
  mutable bool _default_avatar;
};

#endif
