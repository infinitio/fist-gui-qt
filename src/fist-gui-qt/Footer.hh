#ifndef FIST_GUI_QT_FOOTER_HH
# define FIST_GUI_QT_FOOTER_HH

# include <QWidget>
# include <QHBoxLayout>

class Footer:
  public QWidget
{
  typedef QWidget Super;
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
