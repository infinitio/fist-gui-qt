#ifndef SEARCHFIELD_HH
# define SEARCHFIELD_HH

# include <QLineEdit>
# include <QKeyEvent>
# include <QLabel>
# include <QTimer>
# include <QMovie>

# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/ListWidget.hh>

class SearchField:
  public QWidget
{
  class Field:
    public QLineEdit
  {
  public:
    Field(QWidget* parent = nullptr);
    Field(QString const& text, QWidget* parent = nullptr);

  public:
    void
    keyPressEvent(QKeyEvent * event) override;
  };

/*-------------.
| Construction |
`-------------*/
public:
  SearchField(QWidget* owner);

/*--------.
| Display |
`--------*/
public:
  ELLE_ATTRIBUTE(QLabel*, icon);
  ELLE_ATTRIBUTE(Field*, search_field);
public:
  void
  set_icon(QPixmap const& pixmap);

  void
  set_icon(QMovie& movie);

  void
  set_text(QString const& text);

  void
  insert_text(QString const& text);

  QString
  text() const;

  void
  clear();

protected:
  void keyPressEvent(QKeyEvent* event);

Q_SIGNALS:
  void
  up_pressed();

  void
  down_pressed();

  void
  return_pressed();

  void
  search_ready(QString const& text);

private slots:
  void
  delay_expired();

  void
  text_changed(QString const& text);

  void
  focusInEvent(QFocusEvent* event) override;

private:
  QTimer _search_delay;

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;

private:
  Q_OBJECT;
};


#endif
