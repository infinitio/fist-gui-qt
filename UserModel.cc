#include <fist-gui-qt/UserModel.hh>

UserModel::UserModel(gap_State* state,
                     uint32_t id):
  _state(state),
  _id(id),
  _fullname((const char *) nullptr)
{
}

QString const&
UserModel::fullname() const
{
  if (this->_fullname.isNull())
  {
    this->_fullname = gap_user_fullname(this->_state, this->_id);
  }

  // assert this->_fullname is not null.

  return this->_fullname;
}

uint32_t
UserModel::id() const
{
  return this->_id;
}
