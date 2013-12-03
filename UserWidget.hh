#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <iostream>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/UserModel.hh>

class UserWidget:
  public ListItem
{
public:
  UserWidget(UserModel const& model,
             QWidget* parent = nullptr);

Q_SIGNALS:
  void clicked_signal(uint32_t);

public:
  virtual void trigger();

private:
  UserModel const& _model;
  AvatarIcon* _avatar;

  Q_OBJECT;
};

#endif // !USERWIDGET_HH
