#ifndef ROUNDBUTTON_HH
# define ROUNDBUTTON_HH

# include <QPushButton>
# include <QColor>

class RoundButton:
  public QPushButton
{
public:
  RoundButton(QColor const& color,
              QPixmap const& icon = QPixmap(),
              QSize const& size = QSize(35, 35),
              QWidget* parent = nullptr);

private:
  QColor _color;
  QSize _size;
  QPixmap _image;
  bool _hover;

private:
  QSize
  sizeHint() const override;

/*------.
| Hover |
`------*/

protected:
  void
  enterEvent(QEvent*) override;

  void
  leaveEvent(QEvent*) override;

  /*--------.
  | Drawing |
  `--------*/
protected:
  virtual
  void
  paintEvent(QPaintEvent* event);
};

#endif
