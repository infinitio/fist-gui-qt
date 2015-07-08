#ifndef FIST_GUI_QT_SEND_VIEW_USERS_HXX
# define FIST_GUI_QT_SEND_VIEW_USERS_HXX

# include <algorithm>

# include <fist-gui-qt/SendView/Users.hh>

namespace fist
{
  namespace sendview
  {
    template <typename T>
    bool
    Users::_in_recipients(T const& r) const
    {
      return std::find_if(this->_recipients.begin(),
                          this->_recipients.end(),
                          [&] (Recipient const& recipient)
                          {
                            return recipient == r;
                          }) != this->_recipients.end();
    }
  }
}


#endif
