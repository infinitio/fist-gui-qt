#ifndef NONOTIFICATIONWIDGET_HH
# define NONOTIFICATIONWIDGET_HH

# include <fist-gui-qt/ListItem.hh>

# include <QWidget>
# include <QHBoxLayout>

class NoNotificationWidget:
  public ListItem
{
public:
  NoNotificationWidget(QWidget* parent = nullptr);

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
