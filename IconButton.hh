#ifndef ICONBUTTON_HH
# define ICONBUTTON_HH

# include <QPainter>
# include <QPushButton>

# include <fist-gui-qt/utils.hh>

class IconButton: public QPushButton
{
public:
  IconButton(QPixmap const& pixmap, bool shadow);

  Q_PROPERTY(QColor color READ color WRITE setColor);
  Q_PROPERTY_R(QColor, color, color);
  Q_PROPERTY(bool shadow READ hasShadow WRITE setShadow);
  Q_PROPERTY_R(bool, has_shadow, hasShadow);

private:
  void _draw_shape(QPixmap& pixmap, QColor const& color);
  void _refresh();

protected:
  virtual
  QSize
  sizeHint() const override;
  virtual
  void
  paintEvent(QPaintEvent*) override;

private:
  QPixmap _cache;
  QPixmap _original;
  QPixmap _icon;
  QPixmap _shadow;
};


#endif
