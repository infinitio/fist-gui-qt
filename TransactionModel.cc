#include "TransactionModel.hh"

#include <iostream>


#include <QBuffer>
#include <QImageReader>

TransactionModel::TransactionModel(gap_State* state,
                                   uint32_t id):
  _state(state),
  _id(id),
  _peer_fullname((char const*) nullptr),
  _peer_id(0),
  _files(),
  _avatar(),
  _default_avatar(true)
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
    char** to_delete = files;

    do
    {
      this->_files.push_back(QString::fromUtf8(*files));

      // XXX: do it in gap.
      // ::free((void*) *files);
    }
    while((*++files) != nullptr);

    // XXX: do it in gap.
    // ::free((void*) to_delete);
  }

  return this->_files;
}

gap_TransactionStatus
TransactionModel::status() const
{
  return gap_transaction_status(this->_state, this->_id);
}

QPixmap const&
TransactionModel::avatar() const
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
      this->_avatar = QPixmap(QString("resources/avatar1.png"));
    }
  }

  return this->_avatar;
}
