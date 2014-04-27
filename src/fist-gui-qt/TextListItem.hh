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
               QWidget* parent = nullptr);

  QSize
  sizeHint() const override;

  QSize
  minimumSizeHint() const override;

  void
  trigger() override;

private:
  QHBoxLayout*
  _layout;
};

#endif
