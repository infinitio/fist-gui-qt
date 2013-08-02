#ifndef LISTWIDGET_HH
# define LISTWIDGET_HH

# include <QWidget>

# include "SmoothScrollBar.hh"

class ListWidget:
  public QWidget
{
/*------.
| Types |
`------*/
public:
  typedef ListWidget Self;
  typedef QWidget Super;

/*-------------.
| Construction |
`-------------*/
public:
  ListWidget(QWidget* parent = nullptr);

/*--------.
| Widgets |
`--------*/
public:
    void
    addWidget(QWidget* widget);
    void
    clearWidgets();

/*-------.
| Layout |
`-------*/
public:
    virtual
    QSize
    sizeHint() const override;
    virtual
    QSize
    minimumSizeHint() const override;

  public:
    Q_OBJECT;
    Q_PROPERTY(int offset READ offset WRITE setOffset);
    int offset();
  public Q_SLOTS:
    void setOffset(int val);

  private Q_SLOTS:
    void
    _layout();
  private:
    int _height_hint;
    int _width_hint;
    int _offset;
    std::vector<QWidget*> _widgets;
    SmoothScrollBar* _scroll;

/*-------.
| Events |
`-------*/
protected:
  virtual
  void
  resizeEvent(QResizeEvent*) override;
  virtual
  void
  wheelEvent(QWheelEvent* event) override;
  bool _wheel_event;

/*---------.
| Paitning |
`---------*/
protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;
};

#endif
