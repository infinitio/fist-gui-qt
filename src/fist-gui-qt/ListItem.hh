#ifndef LISTITEM_HH
# define LISTITEM_HH

# include <elle/attribute.hh>

# include <QColor>
# include <QWidget>

class ListItem:
  public QWidget
{
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

  virtual void update();

  int
  widthHint() const;

  QSize
  sizeHint() const override;

private:
  virtual
  void
  trigger() = 0;

  ELLE_ATTRIBUTE(QColor, background);
  ELLE_ATTRIBUTE(bool, clickable);
};

#endif // !LISTITEM_HH
