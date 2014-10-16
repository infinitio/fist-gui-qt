#ifndef FIST_GUI_QT_TEXTLISTITEM_HH
# define FIST_GUI_QT_TEXTLISTITEM_HH

# include <elle/Printable.hh>

# include <fist-gui-qt/ListItem.hh>

# include <QWidget>
# include <QLabel>
# include <QHBoxLayout>

class TextListItem:
  public ListItem
{
  typedef ListItem Super;
public:
  TextListItem(QString const& text,
               int height = 40,
               QWidget* parent = nullptr);

  QSize
  sizeHint() const override;

  QSize
  minimumSizeHint() const override;

  void
  trigger() override;
private:
  void
  print(std::ostream& out) const override;

private:
  ELLE_ATTRIBUTE(QHBoxLayout*, layout);
  ELLE_ATTRIBUTE(int, height);
  ELLE_ATTRIBUTE(QLabel*, text);
};

#endif
