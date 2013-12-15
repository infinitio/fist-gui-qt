#ifndef SHAPEBUTTON_HH
# define SHAPEBUTTON_HH

# include <QPainter>
# include <QPushButton>

# include <fist-gui-qt/utils.hh>

# include <functional>

class ShapeButton: public QPushButton
{
  Q_OBJECT;

  typedef std::function<void ()> Callback;
public:
  ShapeButton(QPixmap const& pixmap, bool shadow = false, Callback const& = {});

  Q_PROPERTY(QColor color READ color);
  Q_PROPERTY_R(QColor, color, color);
  Q_PROPERTY(bool shadow READ hasShadow);
  Q_PROPERTY_R(bool, has_shadow, hasShadow);

private:
  void _draw_shape(QPixmap& pixmap, QColor const& color);
  void _refresh();

public:
  void
  enable();

  void
  disable();

protected:
  virtual
  QSize
  sizeHint() const override;
  virtual
  void
  paintEvent(QPaintEvent*) override;

private slots:
  void
  _clicked();

private:
  QPixmap _cache;
  QPixmap _original;
  QPixmap _icon;
  QPixmap _shadow;
  Callback _callback;
};

#endif
