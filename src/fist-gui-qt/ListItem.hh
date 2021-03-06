#ifndef LISTITEM_HH
# define LISTITEM_HH

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/fwd.hh>

# include <QColor>
# include <QWidget>

class ListItem:
  public QWidget,
  public elle::Printable
{
  friend ListWidget;

public:
  ListItem(QWidget* parent = nullptr,
           QColor const& background_color = Qt::transparent,
           bool clickable = true);
  virtual
  ~ListItem() = default;
  void
  mousePressEvent(QMouseEvent* e) override;
  void
  enterEvent(QEvent* e) override;
  void
  leaveEvent(QEvent* e) override;

public:

  virtual
  void
  _update();

public:
  virtual
  int
  widthHint() const;

  QSize
  sizeHint() const override;

public:
  virtual
  void
  trigger() = 0;

  ELLE_ATTRIBUTE(QColor, background);
  ELLE_ATTRIBUTE(bool, clickable);

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

#endif // !LISTITEM_HH
