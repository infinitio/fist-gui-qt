#ifndef LISTITEM_HH
# define LISTITEM_HH

# include <QWidget>

class ListItem:
  public QWidget
{
public:
  ListItem(QWidget* parent = nullptr);
  virtual ~ListItem() = default;
  void mousePressEvent(QMouseEvent* e) override;
  virtual void update();

public:
  virtual void trigger() = 0;
};

#endif // !LISTITEM_HH
