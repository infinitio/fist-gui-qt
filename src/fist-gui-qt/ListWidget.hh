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

/*------.
| Enums |
`------*/
public:
  enum Position
  {
    Top = 0x01,
    Bottom = 0x02,
  };

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
  add_widget(ListItem* widget, Position position = Bottom);

  void
  remove_widget(ListItem* widget, bool all = true);

  void
  move_widget(ListItem* widget, Position position = Bottom);

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
public Q_SLOTS:
  void
  update();

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