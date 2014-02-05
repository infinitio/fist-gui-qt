#ifndef SEARCHRESULTWIDGET_HH
# define SEARCHRESULTWIDGET_HH

# include <QHBoxLayout>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/UserModel.hh>

class SearchResultWidget:
  public ListItem
{
public:
  SearchResultWidget(UserModel const& model,
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
  update() override;

/*-----------.
| Attributes |
`-----------*/
private:
  UserModel const& _model;
  AvatarIcon* _avatar;

/*-------.
| Layout |
`-------*/
private:
  QHBoxLayout* _layout;

  Q_OBJECT;
};

#endif
