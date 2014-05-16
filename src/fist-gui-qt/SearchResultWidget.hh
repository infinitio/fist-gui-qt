#ifndef SEARCHRESULTWIDGET_HH
# define SEARCHRESULTWIDGET_HH

# include <QHBoxLayout>

# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/User.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>

class SearchResultWidget:
  public ListItem
{
  typedef ListItem Super;
public:
  SearchResultWidget(fist::model::User const& model,
                     bool preselected = false,
                     QWidget* parent = nullptr);
  virtual
  ~SearchResultWidget() = default;

  virtual
  QSize
  sizeHint() const;

Q_SIGNALS:
  void
  selected(uint32_t);
  void
  unselected(uint32_t);

public:
  void
  trigger() override;

protected:
  void
  _update() override;

private slots:
  void
  _on_avatar_updated();

void
_selected();

void
_unselected();
/*-----------.
| Attributes |
`-----------*/
private:
  fist::model::User const& _model;
  AvatarIcon* _avatar;
  ELLE_ATTRIBUTE_R(fist::TwoStateIconButton*, selector);

/*-------.
| Layout |
`-------*/
private:
  QHBoxLayout* _layout;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

#endif
