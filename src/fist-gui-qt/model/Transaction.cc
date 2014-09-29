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
      , _concerns_device(gap_transaction_concern_device(this->_state.state(), this->id(), false))
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
      , _mtime()
      , _final(false)
      , _avatar()
      , _default_avatar(true)
      , _new_avatar(true)
    {
      this->_final = this->is_final();
      ELLE_TRACE_SCOPE("%s: create transaction model", *this);
      this->update();
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

    void
    Transaction::update() const
    {
      this->_concerns_device = gap_transaction_concern_device(this->_state.state(), this->id(), false);
      this->_is_recipient = gap_self_id(this->_state.state()) == gap_transaction_recipient_id(this->_state.state(), this->id());

#ifndef FIST_PRODUCTION_BUILD
      this->_tooltip = QString("id: %1, status: %2, (%3) (%4 device)\nsender: %5 (%6)\nrecipient %7 (%8)\n")
        .arg(this->id())
        .arg(this->status())
        .arg(this->is_sender() ? "sender" : "recipient")
        .arg(this->_concerns_device ? "concerns" : "doesn't concern")
        .arg(gap_transaction_sender_id(this->_state.state(), this->id()))
        .arg(gap_transaction_sender_device_id(this->_state.state(), this->id()))
        .arg(gap_transaction_recipient_id(this->_state.state(), this->id()))
        .arg(gap_transaction_recipient_device_id(this->_state.state(), this->id()));
      this->_tooltip.append(this->files_tooltip());
#endif
    }

    QString
    Transaction::files_tooltip() const
    {
      QString tooltip;
      for (auto const& file: this->files())
      {
        tooltip.append(file).append("\n");
      }
      tooltip.remove(tooltip.size() - 1, 1);
      return tooltip;
    }

    bool
    Transaction::is_sender_device() const
    {
      return this->is_sender() && this->_concerns_device;
    }

    bool
    Transaction::is_recipient_device() const
    {
      return this->is_recipient() && this->_concerns_device;
    }

    bool
    Transaction::acceptable() const
    {
      return (this->status() == gap_transaction_waiting_accept && this->is_recipient()) ||
        (this->status() == gap_transaction_on_other_device) && (this->is_sender() && this->is_recipient() && !this->is_sender_device());
    }


    bool
    Transaction::is_final() const
    {
      if (this->_final)
        return true;

      static QVector<gap_TransactionStatus> final_states =
        {
          gap_transaction_finished,
          gap_transaction_failed,
          gap_transaction_canceled,
          gap_transaction_rejected,
          gap_transaction_deleted,
          gap_transaction_cloud_buffered,
        };

      if (final_states.contains(this->status()))
      {
        this->_final = true;
      }
      else if (gap_transaction_is_final(this->_state.state(), this->id()))
      {
        this->_final = true;
      }
      return this->_final;
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
      stream << "Transaction(" << this->id() << ", " << this->status() << ")";
      if (this->_is_sender)
      {
        stream << " "
               << (this->is_sender() ? "to" : "from")
               << " " << this->peer_fullname();
      }
    }
  }
}
