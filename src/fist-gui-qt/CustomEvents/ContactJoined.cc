#include <elle/log.hh>

#include <fist-gui-qt/CustomEvents/Events.hh>
#include <fist-gui-qt/CustomEvents/ContactJoined.hh>

ELLE_LOG_COMPONENT("infinit.FIST.CustomEvents.ContactJoined");

namespace fist
{
  ContactJoined::ContactJoined(uint32_t id, std::string const& contact)
    : QEvent(CONTACT_JOINED)
    , _id(id)
    , _contact(contact)
  {
    ELLE_DEBUG("%s: constructed", *this);
  }

  ContactJoined::~ContactJoined()
  {
    ELLE_DEBUG("%s: destroyed", *this);
  }
}
