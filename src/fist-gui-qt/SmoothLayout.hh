#ifndef SMOOTHLAYOUT_HH
# define SMOOTHLAYOUT_HH

# include <QWidget>
# include <QPropertyAnimation>
# include <QList>
# include <QVBoxLayout>

# include <elle/Printable.hh>

# include <fist-gui-qt/utils.hh>

class SmoothLayout:
  public QWidget,
  public elle::Printable
{
/*------.
| Types |
`------*/
public:
  typedef SmoothLayout Self;
  typedef QWidget Super;

/*-------------.
| Construction |
`-------------*/
public:
  SmoothLayout(QWidget* owner = nullptr,
               int height_animation_duration = 0);

/*-------.
| Layout |
`-------*/
protected:
  virtual
  QSize
  sizeHint() const override;
  virtual
  void
  resizeEvent(QResizeEvent* event) override;
  virtual
  bool
  event(QEvent* event) override;
  virtual
  void
  childEvent(QChildEvent* event) override;
protected slots:
  void
  _layout();
private:
  QList<QWidget*>
  _child_widgets(bool visible_only = false) const;

  // Filter anchor events to detect changes like visibility changed,
  // position changed, modal window spawned...
  bool
  eventFilter(QObject *obj, QEvent *event) override;

public:
  Q_PROPERTY(int heightHint
             READ heightHint
             WRITE setHeightHint
             NOTIFY onHeightHintChanged);
  Q_PROPERTY_R(int, height_hint, heightHint);
  Q_PROPERTY(int widthHint
             READ widthHint
             WRITE setWidthHint
             NOTIFY onWidthHintChanged);
  Q_PROPERTY_R(int, width_hint, widthHint);
  Q_PROPERTY(int maximumHeight
             READ maximumHeight
             WRITE setMaximumHeight
             NOTIFY onMaximumHeightChanged);
  Q_PROPERTY_R(int, maximum_height, maximumHeight);
  Q_PROPERTY(int maximumWidth
             READ maximumWidth
             WRITE setMaximumWidth
             NOTIFY onMaximumWidthChanged);
  Q_PROPERTY_R(int, maximum_width, maximumWidth);
public:
  void setHeightHint(int value);
  void setWidthHint(int value);
  void setMaximumHeight(int value);
  void setMaximumWidth(int value);


Q_SIGNALS:
  void onHeightHintChanged();
  void onWidthHintChanged();
  void onMaximumHeightChanged();
  void onMaximumWidthChanged();
  void
  resized();

private:
  QPropertyAnimation* _height_animation;
  QPropertyAnimation* _width_animation;

private:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};


#endif
