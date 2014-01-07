#ifndef SEARCHFIELD_HH
# define SEARCHFIELD_HH

# include <QLineEdit>
# include <QKeyEvent>
# include <QTimer>

# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/ListWidget.hh>

class SearchField:
  public QLineEdit
{
/*-------------.
| Construction |
`-------------*/
public:
  SearchField(QWidget* owner);

/*--------.
| Display |
`--------*/
public:
  Q_PROPERTY(QPixmap icon READ icon WRITE setIcon);
  Q_PROPERTY_R(QPixmap, icon, icon);
public:
  void
  setIcon(QPixmap const& pixmap);

protected:
  void keyPressEvent(QKeyEvent* event);

Q_SIGNALS:
  void
  up_pressed();

  void
  down_pressed();

  void
  search_ready(QString const& text);

private slots:
  void
  delay_expired();

  void
  text_changed(QString const& text);

private:
  QTimer _search_delay;

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;

/*---------.
| Painting |
`---------*/
public:
  void
  paintEvent(QPaintEvent* event);

private:
  Q_OBJECT;
};


#endif
