#ifndef HORIZONTALSEPARATOR_HH
# define HORIZONTALSEPARATOR_HH

# include <QFrame>
# include <QWidget>

# include <elle/attribute.hh>

class HorizontalSeparator:
  public QFrame
{
public:
  HorizontalSeparator(QWidget* parent = nullptr,
                      int margin = 0);

public:
  QSize
  sizeHint() const override;

protected:
  void
  paintEvent(QPaintEvent*) override;

  ELLE_ATTRIBUTE_RW(int, margin);
};

#endif
