#ifndef FOOTER_HH
# define FOOTER_HH

# include <QWidget>

class Footer:
  public QWidget
{
public:
  Footer(QWidget* parent = nullptr);

protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;
};


#endif
