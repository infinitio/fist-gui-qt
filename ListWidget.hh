#ifndef LISTWIDGET_HH
# define LISTWIDGET_HH

# include <QSet>
# include <QWidget>
# include <QLineEdit>

# include <fist-gui-qt/SmoothScrollBar.hh>
# include <fist-gui-qt/ListItem.hh>

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
  ListWidget(QWidget* parent, QWidget* mate);

/*--------.
| Widgets |
`--------*/
public:
  void
  addWidget(ListItem* widget);

  void
  removeWidget(ListItem* widget);

  QList<ListItem*> const&
  widgets() const;

  void
  clearWidgets();

  void
  setFocus();

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
  void
  set_mate(QWidget* mate);

public:
  Q_OBJECT;
  Q_PROPERTY(int offset READ offset WRITE setOffset);
  int offset();
public Q_SLOTS:
  void setOffset(int val);
public:
  Q_PROPERTY(int _max_rows READ maxRows WRITE setMaxRows);
  int maxRows();
public Q_SLOTS:
  void setMaxRows(int val);

Q_SIGNALS:
  void
  resized();

private Q_SLOTS:
  void
  _layout();
private:
  int _height_hint;
  int _width_hint;
  int _offset;
  int _max_rows;

  QList<ListItem*> _widgets;
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

  void
  keyPressEvent(QKeyEvent* event) override;
  size_t _keyboard_index;
  QWidget* _mate;

/*---------.
| Paitning |
`---------*/
protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;
};

#endif
