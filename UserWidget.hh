#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <iostream>
# include "ListItem.hh"

class UserWidget:
  public ListItem
{
public:
  UserWidget(QString const& user, QWidget* parent);

public:
  virtual void trigger();

private:
  QString const _user;
};

#endif // !USERWIDGET_HH
