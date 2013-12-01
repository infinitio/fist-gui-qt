#ifndef SMOOTHLAYOUT_HH
# define SMOOTHLAYOUT_HH

# include <QWidget>

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
  _child_widgets() const;

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
public:
  void setHeightHint(int value);
  void setWidthHint(int value);
Q_SIGNALS:
  void onHeightHintChanged();
  void onWidthHintChanged();

private:
  Q_OBJECT;
};


#endif
