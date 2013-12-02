#ifndef USERMODEL_HH
# define USERMODEL_HH

# ifndef FAKEGAP
# include <surface/gap/gap.h>
# else
# include <fist-gui-qt/FakeGap.hh>
# endif

# include <QString>

# include <memory>

class UserModel
{
public:
  UserModel(gap_State* state,
            uint32_t id);

  UserModel(UserModel const&) = default;

  QString const&
  fullname() const;

  uint32_t
  id() const;

  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _fullname;

};

#endif
