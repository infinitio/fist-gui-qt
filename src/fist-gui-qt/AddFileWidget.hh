#ifndef ADDFILEITEM_HH
# define ADDFILEITEM_HH

# include <QLabel>
# include <QWidget>
# include <QPropertyAnimation>
# include <QColor>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>
# include <fist-gui-qt/utils.hh>

class GrowingArea:
  public QWidget
{
  typedef QWidget Super;
public:

  enum class State
  {
    expanding,
    expanded,
    shrinking,
    shrinked
  };

  GrowingArea(QWidget* body,
              QWidget* parent = nullptr);

private:
  bool
  eventFilter(QObject *obj, QEvent *event) override;

public slots:
  void
  expand();

  void
  shrink();

  void
  fuuuuuu();

private slots:
  void
  _done();

private:
  Q_PROPERTY(int minimumHeightHint
             READ minimumHeightHint
             WRITE minimumHeightHint
             NOTIFY minimumHeightHintChanged);

public:
  ELLE_ATTRIBUTE_R(QWidget*, body);
  int _minimumHeightHint;
  int minimumHeightHint() const { return this->_minimumHeightHint; }
  void minimumHeightHint(int value) { this->_minimumHeightHint = value; // std::cerr << value << std::endl;
    this->update(); this->repaint(); this->updateGeometry(); emit minimumHeightHintChanged(); }
  QPropertyAnimation* _height_animator;
  State _state;

  QSize
  minimumSizeHint() const override
  {
    return this->sizeHint(); //QSize(Super::minimumSizeHint().width(), this->minimumHeightHint());
  }

  QSize
  sizeHint() const override
  {
    return QSize(this->_body->sizeHint().width(),
                 std::min(this->_body->sizeHint().height(),
                          this->minimumHeightHint()));
  }

signals:
  void minimumHeightHintChanged();
  void shrinking();
  void expanding();

private:
  Q_OBJECT;
};

class AddFileWidget:
  public QWidget,
  public elle::Printable
{
  typedef QWidget Super;

public:
  AddFileWidget(QWidget* body,
                QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const override;

  // int
  // widthHint() const override;

public:
  Q_PROPERTY(QColor pulseColor
             READ pulseColor
             WRITE setPulseColor);
  Q_PROPERTY_R(QColor, pulse_color, pulseColor);

  void
  setPulseColor(QColor const& color);

private:
  void
  enterEvent(QEvent*) override;

  void
  leaveEvent(QEvent*) override;

  void
  mousePressEvent(QMouseEvent*) override;

public slots:
  void
  on_entered();

  void
  on_left();

  void
  dropEvent(QDropEvent*) override;

signals:
   void
   clicked();

   void
   file_dropped(QUrl const&);

public slots:
  void
  pulse();

public:
  QWidget*
  body()
  {
    return this->_growing_area->body();
  }

private:
  ELLE_ATTRIBUTE_R(IconButton*, attach);
  ELLE_ATTRIBUTE_R(QLabel*, text);
  ELLE_ATTRIBUTE_R(fist::TwoStateIconButton*, expanser);
  ELLE_ATTRIBUTE_R(GrowingArea*, growing_area);
  QPropertyAnimation* _pulse_animation;
private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
