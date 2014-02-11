#include <fist-gui-qt/UserModel.hh>

#include <QByteArray>
#include <QBuffer>
#include <QImageReader>

UserModel::UserModel(gap_State* state,
                     uint32_t id):
  _state(state),
  _id(id),
  _fullname((const char *) nullptr),
  _avatar(),
  _default_avatar(true),
  _new_avatar(true)
{
}

QString const&
UserModel::fullname() const
{
  if (this->_fullname.isNull())
  {
    this->_fullname = QString::fromUtf8(gap_user_fullname(this->_state, this->_id));
  }

  // assert this->_fullname is not null.

  return this->_fullname;
}

QString const&
UserModel::handle() const
{
  if (this->_handle.isNull())
  {
    this->_handle = QString::fromUtf8(gap_user_handle(this->_state, this->_id));
  }

  return this->_handle;
}

uint32_t
UserModel::id() const
{
  return this->_id;
}

bool
UserModel::new_avatar() const
{
  return this->_new_avatar;
}

void
UserModel::avatar_available()
{
  this->_new_avatar = true;
}

QPixmap const&
UserModel::avatar() const
{
  if (this->_avatar.isNull() || this->_default_avatar == true)
  {
    /// Get user icon data.
    void* data = nullptr;
    size_t len = 0;

    auto res = gap_avatar(this->_state, this->_id, &data, &len);

    if (res == gap_ok)
    {
      QByteArray raw((char *) data, len);
      QBuffer buff(&raw);
      QImageReader reader;
      reader.setDecideFormatFromContent(true);
      reader.setDevice(&buff);
      this->_avatar =  QPixmap::fromImageReader(&reader);
      this->_default_avatar = false;
    }
    else if(this->_avatar.isNull())
    {
      this->_avatar = QPixmap(QString(":/images/avatar_default.png"));
    }
  }

  this->_new_avatar = false;
  return this->_avatar;
}
