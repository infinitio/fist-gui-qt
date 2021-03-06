#ifndef FIST_GUI_QT_LISTWIDGET_HH
# define FIST_GUI_QT_LISTWIDGET_HH

# include <memory>

# include <boost/optional.hpp>

# include <QSet>
# include <QWidget>
# include <QLineEdit>
# include <QVector>
# include <QColor>

# include <elle/Printable.hh>

# include <fist-gui-qt/SmoothScrollBar.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/globals.hh>

class ListWidget:
  public QWidget,
  public elle::Printable
{
public:
  class Separator
  {
  public:
    Separator(QVector<QColor> const& colors,
              int left_margin = 0,
              int right_margin = 0)
      : _colors(colors)
      , _left_margin(left_margin)
      , _right_margin(right_margin)
    {}

    int
    height() const
    {
      return this->_colors.size();
    }

  private:
    ELLE_ATTRIBUTE(QVector<QColor>, colors);
    ELLE_ATTRIBUTE(int, left_margin);
    ELLE_ATTRIBUTE(int, right_margin);
  friend ListWidget;
  };

/*------.
| Types |
`------*/
public:
  typedef ListWidget Self;
  typedef QWidget Super;
  typedef std::shared_ptr<QWidget> ItemPtr;

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
 ListWidget(QWidget* parent = nullptr,
            Separator const& separator = Separator(list::separator::colors),
            boost::optional<QColor> background_color = {});

/*--------.
| Widgets |
`--------*/
public:
  void
  add_widget(ItemPtr widget, Position position = Bottom);

  void
  remove_widget(ItemPtr widget, bool all = true);

  void
  add_separator();

private:
  // Filter anchor events to detect changes like visibility changed,
  // position changed, modal window spawned...
  bool
  eventFilter(QObject *obj, QEvent *event) override;

public:
  void
  move_widget(ItemPtr widget, Position position = Bottom);

  QList<ItemPtr> const&
  widgets() const;

  void
  clearWidgets();

public slots:
  void
  setFocus();

public:
  int
  index(ItemPtr ptr);

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

private Q_SLOTS:
  void
  _layout();
private:
  int _offset;
  Separator _separator;
  int _separators;
  int _height_hint;
  int _width_hint;
  int _max_rows;

  QList<ItemPtr> _widgets;
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
  void
  _select_element(size_t index);
  size_t _keyboard_index;
  QWidget* _mate;

/*---------.
| Paitning |
`---------*/
protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
