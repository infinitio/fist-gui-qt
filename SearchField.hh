#ifndef SEARCHFIELD_HH
# define SEARCHFIELD_HH

# include <QLineEdit>

# include "utils.hh"

class SearchField:
  public QLineEdit
{
/*-------------.
| Construction |
`-------------*/
public:
  SearchField(QWidget* owner = nullptr);

/*--------.
| Display |
`--------*/
public:
  Q_PROPERTY(QPixmap icon READ icon WRITE setIcon);
  Q_PROPERTY_R(QPixmap, icon, icon);
public:
  void
  setIcon(QPixmap const& pixmap);

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
};


#endif
