#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <iostream>
# include <fist-gui-qt/ListItem.hh>

class UserWidget:
  public ListItem
{
public:
  UserWidget(QString const& user, uint32_t uid, QWidget* parent = nullptr);

Q_SIGNALS:
  void clicked_signal(uint32_t);

public:
  virtual void trigger();

private:
  QString const _user;
  uint32_t _uid;

  Q_OBJECT;
};

#endif // !USERWIDGET_HH
