#include <QString>
#include <QVector>
#include <QStringList>

#include <elle/log.hh>

#include <fist-gui-qt/model/Link.hh>
#include <fist-gui-qt/State.hh>

namespace fist
{
  namespace model
  {
    Link::Link(fist::State& state,
               uint32_t id)
      : Super(state, id)
      , _link(gap_link_transaction_by_id(this->_state.state(), this->id()))
    {
    }

    void
    Link::update()
    {
      surface::gap::LinkTransaction old = this->_link;
      this->_link = gap_link_transaction_by_id(this->_state.state(), this->id());

      if (old.status != this->status())
        emit status_updated();
      if (old.click_count != this->click_count())
        emit click_count_updated();
    }

    QUrl
    Link::url() const
    {
      if (this->_link.link)
        return QUrl(QString::fromStdString(this->_link.link.get()));
      return QUrl();
    }

    QString
    Link::name() const
    {
      return QString::fromStdString(this->_link.name);
    }

    QDateTime const&
    Link::mtime() const
    {
      this->_mtime.setTime_t(this->_link.mtime);
      return this->_mtime;
    }

    uint32_t
    Link::click_count() const
    {
      return this->_link.click_count;
    }

    float
    Link::progress() const
    {
      if (this->status() == gap_transaction_transferring)
      {
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
    Link::is_finished() const
    {
      static QVector<gap_TransactionStatus> final_states =
        {
          gap_transaction_finished,
          gap_transaction_failed,
          gap_transaction_canceled,
        };

      return final_states.contains(this->status()) || gap_transaction_is_final(this->_state.state(), this->id());
    }

    void
    Link::print(std::ostream& stream) const
    {
      stream << "Link(" << this->id() << ")";
    }
  }
}
