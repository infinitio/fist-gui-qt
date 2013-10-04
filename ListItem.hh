#ifndef LISTITEM_HH
# define LISTITEM_HH

# include <QWidget>

class ListItem:
  public QWidget
{
public:
  ListItem(QWidget* parent = nullptr);
  void mousePressEvent(QMouseEvent* e) override;

public:
  virtual bool compare_id(const uint32_t id) = 0;
  virtual void trigger() = 0;
};

#endif // !LISTITEM_HH
