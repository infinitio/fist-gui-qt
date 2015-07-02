#ifndef FIST_GUI_QT_CUSTOM_EVENTS_ACCOUNTUPDATED_HH
# define FIST_GUI_QT_CUSTOM_EVENTS_ACCOUNTUPDATED_HH

# include <QEvent>

# include <elle/attribute.hh>

# include <surface/gap/gap.hh>

namespace fist
{
  class AccountUpdated:
    public QEvent
  {
  public:
    AccountUpdated(::Account const& account);

    private:
    ELLE_ATTRIBUTE_R(::Account, account);
  };
}

#endif
