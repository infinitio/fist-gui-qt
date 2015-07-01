#ifndef FIST_GUI_QT_SEND_VIEW_USERS_HXX
# define FIST_GUI_QT_SEND_VIEW_USERS_HXX

# include <algorithm>

# include <elle/log.hh>

# include <fist-gui-qt/SendView/Users.hh>

namespace fist
{
  namespace sendview
  {
    template <typename T>
    bool
    Users::_in_recipients(T const& r) const
    {
      ELLE_LOG_COMPONENT("infinit.FIST.SendView.Users");
      ELLE_DEBUG_SCOPE("search %s in %s", r, this->_recipients);
      return std::find_if(this->_recipients.begin(),
                          this->_recipients.end(),
                          [&] (Recipient const& recipient)
                          {
                            ELLE_DUMP("recpient: %s", recipient);
                            return recipient == r;
                          }) != this->_recipients.end();
    }
  }
}


#endif
