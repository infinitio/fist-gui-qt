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
  virtual bool compare_id(const uint32_t uid);
  virtual void trigger();

private:
  uint32_t _uid;
  QString const _user;
};

#endif // !USERWIDGET_HH
