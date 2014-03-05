#ifndef ADDFILEITEM_HH
# define ADDFILEITEM_HH

# include <QLabel>
# include <QWidget>
# include <QColor>

# include <elle/Printable.hh>

# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/utils.hh>

class AddFileWidget:
  public QWidget,
  public elle::Printable
{
public:
  AddFileWidget(QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const override;

  IconButton*
  attach();

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
  on_drag_entered();

  void
  on_drag_left();

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

private:
  QLabel* _text;
  IconButton* _attach;

private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
