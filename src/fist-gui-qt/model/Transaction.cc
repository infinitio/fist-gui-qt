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
    static
    surface::gap::PeerTransaction
    peer_transaction(fist::State& state,
                     uint32_t id)
    {
      surface::gap::PeerTransaction transaction;
      auto res = gap_peer_transaction_by_id(state.state(), id, transaction);
      if (res != gap_ok); // XXX
      return transaction;
    }

    Transaction::Transaction(fist::State& state,
                             uint32_t id)
      : Transaction(state, peer_transaction(state, id))
    {}

    Transaction::Transaction(fist::State& state,
                             surface::gap::PeerTransaction const& transaction)
      : Super(state, transaction.id)
      , _transaction(transaction)
      , _mtime()
      , _pause(false)
    {
      ELLE_DEBUG_SCOPE("%s: construction", *this);
      this->mtime(transaction.mtime);
    }

    bool
    Transaction::transaction(surface::gap::PeerTransaction const& transaction)
    {
      surface::gap::PeerTransaction old = this->_transaction;
      this->_transaction = transaction;
      this->mtime(this->_transaction.mtime);
      if (this->status() == gap_transaction_paused)
        this->_pause = true;
      else
        this->_pause = false;
      emit status_updated();
      return old.status != this->status();
    }

    bool
    Transaction::is_sender() const
    {
      return this->_transaction.sender_id == this->_state.me().id();
    }

    bool
    Transaction::is_recipient() const
    {
      return this->_transaction.recipient_id == this->_state.me().id();
    }

    bool
    Transaction::is_sender_device() const
    {
      return this->is_sender() &&
        this->_transaction.sender_device_id == this->_state.device_id();
    }

    bool
    Transaction::has_recipient_device() const
    {
      return !this->_transaction.recipient_device_id.empty();
    }

    bool
    Transaction::is_recipient_device() const
    {
      return this->is_recipient() &&
        this->_transaction.recipient_device_id == this->_state.device_id();
    }

    bool
    Transaction::concerns_device() const
    {
      return this->is_sender_device() || this->is_recipient_device();
    }

    model::User const&
    Transaction::peer() const
    {
      return this->_state.user(
        this->is_sender()
        ? this->_transaction.recipient_id
        : this->_transaction.sender_id);
    }

    void
    Transaction::on_peer_changed() const
    {
      emit peer_changed();
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
    Transaction::acceptable() const
    {
      return this->status() == gap_transaction_waiting_accept
        && this->is_recipient()
        && !this->is_sender_device();
    }

    bool
    Transaction::running() const
    {
      return !this->is_final() &&
        this->status() != gap_transaction_waiting_accept;
    }

    bool
    Transaction::is_final() const
    {
      static QVector<gap_TransactionStatus> sender_final_states = {
        gap_transaction_finished,
        gap_transaction_failed,
        gap_transaction_canceled,
        gap_transaction_rejected,
        gap_transaction_deleted,
        gap_transaction_cloud_buffered,
      };

      static QVector<gap_TransactionStatus> recipient_final_states = {
        gap_transaction_finished,
        gap_transaction_failed,
        gap_transaction_canceled,
        gap_transaction_rejected,
        gap_transaction_deleted,
      };

      return this->is_sender()
        ? sender_final_states.contains(this->status())
        : recipient_final_states.contains(this->status());
    }

    float
    Transaction::progress() const
    {
      return gap_transaction_progress(this->_state.state(), this->id());
    }

    QVector<QString> const&
    Transaction::files() const
    {
      if (this->_files.isEmpty())
      {
        for (auto const& file: this->_transaction.file_names)
          this->_files.append(QString_from_utf8_string(file));
      }
      return this->_files;
    }

    gap_TransactionStatus
    Transaction::status() const
    {
      return this->_transaction.status;
    }

    void
    Transaction::mtime(double time)
    {
      if (time > 0)
        this->_mtime = QDateTime::fromTime_t(time);
      else
        this->_mtime = QDateTime::currentDateTime();
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
      stream << "Transaction(" << this->_transaction << ")";
    }
  }
}
