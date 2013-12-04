#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <QHBoxLayout>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/UserModel.hh>

class UserWidget:
  public ListItem
{
public:
  UserWidget(UserModel const& model,
             QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const;

Q_SIGNALS:
  void clicked_signal(uint32_t);

public:
  virtual void trigger();

/*-------.
| Layout |
`-------*/
private:
  QHBoxLayout* _layout;

/*-----------.
| Attributes |
`-----------*/
private:
  UserModel const& _model;
  AvatarIcon* _avatar;

  Q_OBJECT;
};

#endif // !USERWIDGET_HH
