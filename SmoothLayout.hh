#ifndef SMOOTHLAYOUT_HH
# define SMOOTHLAYOUT_HH

# include <QWidget>
# include <QPropertyAnimation>

# include <fist-gui-qt/utils.hh>

class SmoothLayout:
  public QWidget
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
  SmoothLayout(QWidget* owner = nullptr);

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
private:
  void
  _layout();
  QWidgetList
  _child_widgets(bool visible_only = false) const;

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

private:
  QPropertyAnimation* _height_animation;
  QPropertyAnimation* _width_animation;

private:
  Q_OBJECT;
};


#endif
