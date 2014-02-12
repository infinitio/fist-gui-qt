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
  _peer_fullname((char const*) nullptr),
  _peer_id(0),
  _files(),
  _avatar(),
  _default_avatar(true),
  _new_avatar(true)
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

QString const&
TransactionModel::peer_fullname() const
{
  if (this->_peer_fullname.isNull())
  {
    this->_peer_fullname =
      QString::fromUtf8(
        this->is_sender()
        ? gap_transaction_recipient_fullname(this->_state, this->_id)
        : gap_transaction_sender_fullname(this->_state, this->_id)).trimmed();
    ELLE_DEBUG("%s: fetched 'peer_fullname': %s", *this, this->_peer_fullname);
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

void
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
      ELLE_DEBUG("%s: get avatar data", *this);
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
      ELLE_DEBUG("%s: avatar not available yet", *this);
      this->_avatar = QPixmap(QString(":/images/avatar_default.png"));
    }
  }

  this->_new_avatar = false;
  return this->_avatar;
}

void
TransactionModel::print(std::ostream& stream) const
{
  stream << "TransactionModel(" << this->_id << ")";
  if (!boost::logic::indeterminate(this->_is_sender))
  {
    stream << " "
           << (this->is_sender() ? "to" : "from")
           << " " << this->peer_fullname();
  }
}
