#include "TransactionModel.hh"

TransactionModel::TransactionModel(gap_State* state,
                                   uint32_t id):
  _state(state),
  _id(id),
  _peer_fullname((char const*) nullptr),
  _peer_id(0),
  _files()
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
      ? gap_transaction_sender_id(this->_state, this->_id)
      : gap_transaction_recipient_id(this->_state, this->_id);
  }

  return this->_peer_id;
}

QString const&
TransactionModel::peer_fullname() const
{
  if (this->_peer_fullname.isNull())
  {
    this->_peer_fullname = this->is_sender()
      ? gap_transaction_sender_fullname(this->_state, this->_id)
      : gap_transaction_recipient_fullname(this->_state, this->_id);
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
      this->_files.push_back(*files);

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

QPixmap// const&
TransactionModel::avatar() const
{
  return QPixmap(QString("resources/avatar1.png"));
}
