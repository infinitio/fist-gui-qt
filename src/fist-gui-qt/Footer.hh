#ifndef FOOTER_HH
# define FOOTER_HH

# include <QWidget>
# include <QHBoxLayout>

class Footer:
  public QWidget
{
public:
  Footer(QWidget* parent = nullptr);
  virtual ~Footer() = default;

protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;

public:
  QSize
  sizeHint() const override;

  virtual
  QSize
  minimumSizeHint() const override;

protected:
  QHBoxLayout* _layout;
};


#endif