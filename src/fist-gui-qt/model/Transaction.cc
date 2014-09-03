#include <fist-gui-qt/model/Transaction.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/State.hh>

#include <elle/log.hh>

#include <QBuffer>
#include <QImageReader>

ELLE_LOG_COMPONENT("infinit.FIST.model.Transaction");

namespace fist
{
  namespace model
  {
    Transaction::Transaction(fist::State& state,
                             uint32_t id)
      : Model(state, id)
      , _is_sender(
        gap_self_id(this->_state.state()) ==
        gap_transaction_sender_id(this->_state.state(), this->id()))
      , _is_recipient(
        gap_self_id(this->_state.state()) ==
        gap_transaction_recipient_id(this->_state.state(), this->id()))
      , _status(gap_transaction_status(this->_state.state(), this->id()))
      , _peer_fullname((char const*) nullptr)
      , _peer_id(
        this->_is_sender
        ? gap_transaction_recipient_id(this->_state.state(), this->id())
        : gap_transaction_sender_id(this->_state.state(), this->id()))
      , _files()
      , _avatar()
      , _default_avatar(true)
      , _new_avatar(true)
    {
      ELLE_TRACE_SCOPE("%s: create transaction model", *this);
      if (this->is_sender())
        ELLE_DEBUG("as sender");
      if (this->is_recipient())
        ELLE_DEBUG("as recipient");
      ELLE_DEBUG("status %s", this->_status);
    }

    model::User const&
    Transaction::peer() const
    {
      return this->_state.user(this->peer_id());
    }

    bool
    Transaction::is_final() const
    {
      static QVector<gap_TransactionStatus> final_states =
        {
          gap_transaction_finished,
          gap_transaction_failed,
          gap_transaction_canceled,
          gap_transaction_rejected,
          gap_transaction_deleted,
          gap_transaction_cloud_buffered,
        };

      return final_states.contains(this->status()) ||
        gap_transaction_is_final(this->_state.state(), this->id());
    }

    QString const&
    Transaction::peer_fullname() const
    {
      return this->peer().fullname();
    }

    gap_UserStatus
    Transaction::peer_connection_status() const
    {
      return gap_user_status(this->_state.state(), this->peer_id());
    }

    float
    Transaction::progress() const
    {
      return gap_transaction_progress(this->_state.state(), this->id());
    }

    QVector<QString> const&
    Transaction::files() const
    {
      if (this->_files.empty())
      {
        char** files = gap_transaction_files(this->_state.state(), this->id());
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

    bool
    Transaction::concerns_device() const
    {
      return gap_transaction_concern_device(
        this->_state.state(), this->id(), false);
    }

    QString
    Transaction::tooltip() const
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
    Transaction::status() const
    {
      return this->_status;
    }

    void
    Transaction::status(gap_TransactionStatus status)
    {
      this->_status = status;
    }

    QDateTime const&
    Transaction::mtime() const
    {
      this->_mtime.setTime_t(gap_transaction_mtime(this->_state.state(), this->id()));
      return this->_mtime;
    }

    bool
    Transaction::new_avatar() const
    {
      return this->_new_avatar;
    }

    void
    Transaction::avatar_available() const
    {
      this->avatar();
    }

    QPixmap const&
    Transaction::avatar() const
    {
      if (this->_avatar.isNull() || this->_default_avatar == true)
      {
        /// Get user icon data.
        void* data = nullptr;
        size_t len = 0;

        if (gap_ok == gap_avatar(this->_state.state(), this->peer_id(), &data, &len))
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
            this->_avatar = QPixmap(QString(":/avatar_default"));
          }
          this->_default_avatar = false;
        }
        else if(this->_avatar.isNull())
        {
          ELLE_DEBUG("%s: avatar not available yet", *this);
          this->_avatar = QPixmap(QString(":/avatar_default"));
        }
      }

      this->_new_avatar = false;
      return this->_avatar;
    }

    bool
    Transaction::operator ==(Transaction const& t) const
    {
      return this->id() == t.id();
    }

    bool
    Transaction::operator <(Transaction const& t) const
    {
      return this->id() < t.id();
    }

    void
    Transaction::print(std::ostream& stream) const
    {
      stream << "Transaction(" << this->id() << ")";
      if (!boost::logic::indeterminate(this->_is_sender))
      {
        stream << " "
               << (this->is_sender() ? "to" : "from")
               << " " << this->peer_fullname();
      }
    }
  }
}
