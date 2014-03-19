#include <fist-gui-qt/TransactionModel.hh>
#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QBuffer>
#include <QImageReader>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionModel");

TransactionModel::TransactionModel(gap_State* state,
                                   uint32_t id):
  _state(state),
  _id(id),
  _is_sender(boost::logic::indeterminate),
  _peer_id(0),
  _files(),
  _tooltip(),
  _message()
{
  ELLE_TRACE_SCOPE("%s: create transaction model", *this);
}

uint32_t
TransactionModel::id() const
{
  return this->_id;
}

bool
TransactionModel::is_sender() const
{
  if (boost::logic::indeterminate(this->_is_sender))
  {
    this->_is_sender = (gap_self_id(this->_state) ==
                        gap_transaction_sender_id(this->_state, this->_id));
    ELLE_DEBUG("%s: fetched 'is_sender': %s", *this, this->_is_sender);
  }

  return (this->_is_sender == true);
}

uint32_t
TransactionModel::peer_id() const
{
  if (this->_peer_id == 0)
  {
    this->_peer_id = this->is_sender()
      ? gap_transaction_recipient_id(this->_state, this->_id)
      : gap_transaction_sender_id(this->_state, this->_id);
    ELLE_DEBUG("%s: fetched 'peer_id': %s", *this, this->_peer_id);
  }

  return this->_peer_id;
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

    ELLE_DEBUG("%s: fetched 'files': %s", *this, this->_files);
  }

  return this->_files;
}

QString
TransactionModel::tooltip() const
{
  if (this->_tooltip.isNull())
  {
    for (auto const& file: this->files())
      this->_tooltip.append(file).append("\n");
    this->_tooltip.remove(this->_tooltip.size() - 1, 1);

    ELLE_DEBUG("%s: fetched 'tooltip': %s", *this, this->_tooltip);
  }

  return this->_tooltip;
}

QString const&
TransactionModel::message() const
{
  if (this->_message.isNull())
  {
    this->_message =
      QString::fromUtf8(gap_transaction_message(this->_state, this->_id));

    ELLE_DEBUG("fetched 'message': %s", this->_message);

  }
  return this->_message;
}

gap_TransactionStatus
TransactionModel::status() const
{
  return gap_transaction_status(this->_state, this->_id);
}

QDateTime const&
TransactionModel::mtime() const
{
  this->_mtime.setTime_t(gap_transaction_mtime(this->_state, this->_id));
  return this->_mtime;
}

bool
TransactionModel::operator ==(TransactionModel const& t) const
{
  return this->id() == t.id();
}

bool
TransactionModel::operator <(TransactionModel const& t) const
{
  return this->id() < t.id();
}

void
TransactionModel::print(std::ostream& stream) const
{
  stream << "TransactionModel(" << this->_id << ")";
}
