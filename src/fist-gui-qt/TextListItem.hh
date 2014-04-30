#ifndef FIST_GUI_QT_TEXTLISTITEM_HH
# define FIST_GUI_QT_TEXTLISTITEM_HH

# include <fist-gui-qt/ListItem.hh>

# include <QWidget>
# include <QHBoxLayout>

class TextListItem:
  public ListItem
{
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
  ELLE_ATTRIBUTE(QHBoxLayout*, layout);
  ELLE_ATTRIBUTE(int, height);
};

#endif
