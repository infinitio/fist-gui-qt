#include "TransactionModel.hh"

#include <QBuffer>
#include <QImageReader>

TransactionModel::TransactionModel(gap_State* state,
                                   uint32_t id):
  _state(state),
  _id(id),
  _peer_fullname(),
  _peer_id(0),
  _files(),
  _avatar(),
  _default_avatar(true),
  _new_avatar(true)
{
}

uint32_t
TransactionModel::id() const
{
  return this->_id;
}

bool
TransactionModel::is_sender() const
{
  // XXX: Use tribool.
  return gap_self_id(this->_state) ==
    gap_transaction_sender_id(this->_state, this->_id);
}

uint32_t
TransactionModel::peer_id() const
{
  if (this->_peer_id == 0)
  {
    this->_peer_id = this->is_sender()
      ? gap_transaction_recipient_id(this->_state, this->_id)
      : gap_transaction_sender_id(this->_state, this->_id);
  }

  return this->_peer_id;
}

QString const&
TransactionModel::peer_fullname() const
{
  if (this->_peer_fullname.isNull())
  {
    this->_peer_fullname =
      QString::fromUtf8(
        this->is_sender()
        ? gap_transaction_recipient_fullname(this->_state, this->_id)
        : gap_transaction_sender_fullname(this->_state, this->_id));
  }

  return this->_peer_fullname;
}

gap_UserStatus
TransactionModel::peer_connection_status() const
{
  return gap_user_status(this->_state, this->peer_id());
}

float
TransactionModel::progress() const
{
  return gap_transaction_progress(this->_state, this->_id);
}

QVector<QString> const&
TransactionModel::files() const
{
  if (this->_files.empty())
  {
    char** files = gap_transaction_files(this->_state, this->_id);
    char** copyied = files;
    while (*copyied != nullptr)
    {
      this->_files.push_back(QString::fromUtf8(*copyied));
      ++copyied;
    }
    ::free((void*) files);
  }

  return this->_files;
}

gap_TransactionStatus
TransactionModel::status() const
{
  return gap_transaction_status(this->_state, this->_id);
}

bool
TransactionModel::new_avatar() const
{
  return this->_new_avatar;
}

bool
TransactionModel::avatar_available()
{
  this->_new_avatar = true;
}

QPixmap const&
TransactionModel::avatar() const
{
  if (this->_avatar.isNull() || this->_default_avatar == true)
  {
    /// Get user icon data.
    void* data = nullptr;
    size_t len = 0;

    if (gap_ok == gap_avatar(this->_state, this->peer_id(), &data, &len))
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
      this->_avatar = QPixmap(QString("resources/avatar1.png"));
    }
  }

  this->_new_avatar = false;
  return this->_avatar;
}
