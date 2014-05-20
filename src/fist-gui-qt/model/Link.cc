#include <QString>
#include <QStringList>

#include <elle/log.hh>

#include <fist-gui-qt/model/Link.hh>
#include <fist-gui-qt/State.hh>

ELLE_LOG_COMPONENT("infinit.FIST.model.Link");

namespace fist
{
  namespace model
  {
    Link::Link(fist::State& state,
               uint32_t id)
      : Super(state, id)
      , _link(gap_link_transaction_by_id(this->_state.state(), id))
    {}

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
      return QUrl(QString::fromStdString(this->_link.link));
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
      return this->is_finished() ? 1.0f : 0.5f;
    }

    gap_TransactionStatus
    Link::status() const
    {
      return this->_link.status;
    }

    bool
    Link::is_finished() const
    {
      return this->status() != gap_transaction_transferring;
    }

    void
    Link::print(std::ostream& stream) const
    {
      stream << "Link(" << this->id() << ")";
    }
  }
}
