#ifndef ROUNDSHADOWWIDGET_HH
# define ROUNDSHADOWWIDGET_HH

# include <QMainWindow>
# include <QWidget>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

class RoundShadowWidget:
  public QMainWindow,
  public elle::Printable
{
/*------.
| Types |
`------*/
public:
  typedef QMainWindow Super;

/*-------------.
| Construction |
`-------------*/
public:
  RoundShadowWidget(int radius = 5,
                    int shadow = 3,
                    Qt::WindowFlags = Qt::WindowStaysOnTopHint |
                                      Qt::FramelessWindowHint);
/*-------------------.
| Display properties |
`-------------------*/
public:
  Q_OBJECT;
  Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY onRadiusChanged);
  Q_PROPERTY(int shadow READ shadow WRITE setShadow NOTIFY onShadowChanged);
  Q_PROPERTY(QColor background
             READ background
             WRITE setBackground
             NOTIFY onBackgroundChanged);
public:
  void
  setRadius(int value);
  void
  setShadow(int value);
  QColor const&
  background();

public Q_SLOTS:
  void
  setBackground(QColor const& value);

Q_SIGNALS:
  void
  onRadiusChanged();
  void
  onShadowChanged();
  void
  onBackgroundChanged();
private:
  ELLE_ATTRIBUTE_R(int, radius);
  ELLE_ATTRIBUTE_R(int, shadow);

/*---------.
| Geometry |
`---------*/
Q_SIGNALS:
  void onSizeChanged();
protected:
  virtual
  bool
  event(QEvent* event) override;

public:
  void
  resizeEvent(QResizeEvent* event) override;

/*--------.
| Drawing |
`--------*/
protected:
  virtual
  void
  paintEvent(QPaintEvent*);
private:
  void
  _draw_side(QPainter& painter,
             int left,
             int top,
             int width,
             int height);
  void
  _draw_corner(QPainter& painter,
               int left,
               int top,
               int angle);

/*----------.
| Printable |
`----------*/
private:
  void
  print(std::ostream& stream) const override;
};

#endif
