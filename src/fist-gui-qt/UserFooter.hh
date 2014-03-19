#ifndef USERFOOTER_HH
# define USERFOOTER_HH

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>

#include <QHBoxLayout>
#include <QSpacerItem>

class UserFooter:
  public Footer
{
public:
  UserFooter();

  virtual
  ~UserFooter() = default;

  IconButton const&
  menu() const;

  IconButton const&
  send() const;

private:
  IconButton* _menu;
  IconButton* _send;
};

#endif

