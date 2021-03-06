#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <QHBoxLayout>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/User.hh>

class UserWidget:
  public ListItem
{
public:
  UserWidget(fist::model::User const& model,
             QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const;

Q_SIGNALS:
  void clicked_signal(uint32_t);

public:
  void
  trigger() override;

  void
  _update() override;

/*-----------.
| Attributes |
`-----------*/
private:
  fist::model::User const& _model;
  AvatarIcon* _avatar;

/*-------.
| Layout |
`-------*/
private:
  QHBoxLayout* _layout;

  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif // !USERWIDGET_HH
