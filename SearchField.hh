#ifndef SEARCHFIELD_HH
# define SEARCHFIELD_HH

# include <QLineEdit>
# include <QKeyEvent>

# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/ListWidget.hh>

class SearchField:
  public QLineEdit
{
/*-------------.
| Construction |
`-------------*/
public:
  SearchField(QWidget* owner, ListWidget** list);

/*--------.
| Display |
`--------*/
public:
  Q_PROPERTY(QPixmap icon READ icon WRITE setIcon);
  Q_PROPERTY_R(QPixmap, icon, icon);
public:
  void
  setIcon(QPixmap const& pixmap);

protected:
  void keyPressEvent(QKeyEvent* event);

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;

/*---------.
| Painting |
`---------*/
public:
  void
  paintEvent(QPaintEvent* event);

private:
  ListWidget** _list;
};


#endif
