#ifndef HORIZONTALSEPARATOR_HH
# define HORIZONTALSEPARATOR_HH

# include <QFrame>
# include <QWidget>

class HorizontalSeparator:
  public QFrame
{
public:
  HorizontalSeparator(QWidget* parent = nullptr);

public:
  QSize
  sizeHint() const override;

protected:
  void
  paintEvent(QPaintEvent*) override;
};

#endif
