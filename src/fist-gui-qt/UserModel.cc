#include <QByteArray>
#include <QBuffer>
#include <QImageReader>

#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/UserModel.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.UserModel");

UserModel::UserModel(gap_State* state,
                     uint32_t id)
  : _state(state)
  , _id(id)
  , _fullname()
  , _handle()
  , _avatar()
  , _transactions()
  , _default_avatar(true)
  , _new_avatar(true)
{
  ELLE_TRACE_SCOPE("%s: create user model", *this);
}

QString const&
UserModel::fullname() const
{
  if (this->_fullname.isNull())
  {
    this->_fullname =
      QString::fromUtf8(gap_user_fullname(this->_state, this->_id));

    ELLE_DEBUG("%s: fetched 'fullname': %s", *this, this->_fullname);
  }

  // assert this->_fullname is not null.

  return this->_fullname;
}

QString const&
UserModel::handle() const
{
  if (this->_handle.isNull())
  {
    this->_handle =
      QString::fromUtf8(gap_user_handle(this->_state, this->_id));

    ELLE_DEBUG("%s: fetched 'handle': %s", *this, this->_handle);
  }

  return this->_handle;
}

UserModel::Transactions const&
UserModel::transactions() const
{
  ELLE_TRACE_SCOPE("%s: get transactions", *this);

  if (this->_transactions.empty())
  {
    uint32_t* trs = gap_transactions(this->_state);

    for (uint32_t v = 0; trs[v] != gap_null(); v += 1)
      if (this->id() == gap_transaction_sender_id(this->_state, trs[v]) ||
          this->id() == gap_transaction_recipient_id(this->_state, trs[v]))
        this->_transactions.emplace(new TransactionModel(this->_state, trs[v]));

    gap_transactions_free(trs);
    ELLE_DEBUG("fetched 'transactions': %s", this->_transactions);
  }

  return this->_transactions;
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
      if (len > 0) // An avatar is avalaible. If not, keep the default.
      {
        ELLE_DEBUG("%s: get avatar data", *this);
        QByteArray raw((char *) data, len);
        QBuffer buff(&raw);
        QImageReader reader;
        reader.setDecideFormatFromContent(true);
        reader.setDevice(&buff);
        this->_avatar =  QPixmap::fromImageReader(&reader);
      }
      else if(this->_avatar.isNull())
      {
        this->_avatar = QPixmap(QString(":/images/avatar_default.png"));
      }
      emit avatar_updated();
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
UserModel::print(std::ostream& stream) const
{
  stream << "UserModel(" << this->_id << ")";
  if (!this->_fullname.isNull())
    stream << " " << this->fullname();
}
