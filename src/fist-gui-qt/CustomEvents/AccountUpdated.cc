#include <fist-gui-qt/CustomEvents/Events.hh>
#include <fist-gui-qt/CustomEvents/AccountUpdated.hh>

namespace fist
{
  AccountUpdated::AccountUpdated(::Account const& account)
    : QEvent(ACCOUNT_UPDATED)
    , _account(account)
  {
  }
}
