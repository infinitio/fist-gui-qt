#include <fist-gui-qt/CustomEvents/Events.hh>
#include <fist-gui-qt/CustomEvents/AccountUpdated.hh>

ELLE_LOG_COMPONENT("infinit.FIST.events.AccountUpated")

namespace fist
{
  AccountUpdated::AccountUpdated(::Account const& account)
    : QEvent(ACCOUNT_UPDATED)
    , _account(account)
  {
    ELLE_DEBUG("%s: account updated: %s", *this, account);
  }
}
