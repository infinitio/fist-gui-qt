#include <QApplication>
#include <QClipboard>
#include <QString>
#include <QVector>
#include <QStringList>

#include <elle/log.hh>

#include <fist-gui-qt/model/Link.hh>
#include <fist-gui-qt/State.hh>


ELLE_LOG_COMPONENT("infinit.FIST.model.Link");

namespace fist
{
  namespace model
  {
    static
    surface::gap::LinkTransaction
    link_transaction(fist::State& state,
                     uint32_t id)
    {
      surface::gap::LinkTransaction link;
      auto res = gap_link_transaction_by_id(state.state(), id, link);
      if (res != gap_ok); // XXX
      return link;
    }

    Link::Link(fist::State& state, uint32_t id)
      : Link(state, link_transaction(state, id))
    {}

    Link::Link(fist::State& state,
               surface::gap::LinkTransaction const& transaction)
      : Super(state, transaction.id)
      , _link(transaction)
      , _mtime(QDateTime::fromTime_t(transaction.mtime))
    {
    }

    static
    bool
    _url_available(gap_TransactionStatus status)
    {
      static QVector<gap_TransactionStatus> url_available_states =
        {
          gap_transaction_transferring,
          gap_transaction_finished
        };
      return url_available_states.contains(status);
    }

    void
    Link::link(surface::gap::LinkTransaction const& new_link)
    {
      ELLE_TRACE_SCOPE("%s: update", *this);
      surface::gap::LinkTransaction old = this->_link;
      this->_link = new_link;
      ELLE_DEBUG("%s -> %s", old, this->_link);
      this->_mtime = QDateTime::fromTime_t(this->_link.mtime);
      if (old.status != this->status())
      {
        emit status_updated();
        if (this->rejected())
          emit payment_required();
      }
      if (!_url_available(old.status) && _url_available(this->status()))
      {
        if (this->sender_device_id() == this->_state.device().id())
          emit url_available(this->id());
      }
      if (old.click_count != this->click_count())
        emit click_count_updated();
      if (!old.link && !this->url().isEmpty())
      {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(this->url().toString());
      }
    }

    QUrl
    Link::url(bool logged_in) const
    {
      if (this->_link.link)
      {
        auto link = QString::fromStdString(this->_link.link.get());
        if (logged_in)
          link += "?session_id=" + QUrl::toPercentEncoding(this->_state.session_id());
        return QUrl(link);
      }
      return QUrl();
    }

    QString
    Link::name() const
    {
      return QString::fromUtf8(this->_link.name.c_str());
    }

    uint32_t
    Link::click_count() const
    {
      return this->_link.click_count;
    }

    uint64_t
    Link::size() const
    {
      return this->_link.size;
    }

    bool
    Link::rejected() const
    {
      return this->status() == gap_transaction_payment_required;
    }

    QString
    Link::sender_device_id() const
    {
      return QString::fromStdString(this->_link.sender_device_id);
    }

    float
    Link::progress() const
    {
      ELLE_DEBUG_SCOPE("%s: ask for progress", *this);
      if (this->status() == gap_transaction_transferring)
      {
        ELLE_DEBUG("ask gap")
          return gap_transaction_progress(this->_state.state(), this->id());
      }
      else if (this->is_finished())
        return 1.0f;
      else
        return 0.0f;
    }

    gap_TransactionStatus
    Link::status() const
    {
      return this->_link.status;
    }

    bool
    Link::unavailable() const
    {
      static QVector<gap_TransactionStatus> final_states =
        {
          gap_transaction_failed,
          gap_transaction_canceled,
          gap_transaction_deleted,
        };
      return final_states.contains(this->status()) || this->rejected();
    }

    bool
    Link::is_finished() const
    {
      if (this->unavailable() || (gap_transaction_finished == this->status()))
        return true;
      return false;
    }

    void
    Link::print(std::ostream& stream) const
    {
      stream << "Link(" << this->id() << ", " << this->status() << ")";
    }
  }
}
