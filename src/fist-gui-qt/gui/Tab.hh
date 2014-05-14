#ifndef FIST_GUI_QT_GUI_TAB_HH
# define FIST_GUI_QT_GUI_TAB_HH

# include <QObject>
# include <QPushButton>
# include <QVector>
# include <QColor>

# include <elle/attribute.hh>

class Tabber;

class Tab:
  public QWidget
{
  typedef QWidget Super;

public:
  Tab(Tabber& tabber,
      QString const& name,
      QVector<QWidget*> const& widgets);

  virtual
  ~Tab() = default;

private:
  void
  enterEvent(QEvent* event) override;

  void
  leaveEvent(QEvent* event) override;

  void
  paintEvent(QPaintEvent* event) override;

  void
  mouseReleaseEvent(QMouseEvent* event) override;

  QSize
  sizeHint() const override;

  QSize
  minimumSizeHint() const override;

private:
  void
  _hover();

  void
  _active();

  void
  _inactive();


private slots:
  void
    _notify_tabber();

public slots:
  void
    enable();

  void
    disable();

private:
  ELLE_ATTRIBUTE(Tabber&, tabber);
  ELLE_ATTRIBUTE_R(QString, name)
  ELLE_ATTRIBUTE(QVector<QWidget*>, widgets);
  ELLE_ATTRIBUTE_Rw(QColor, color);
private:
  Q_OBJECT;

private:
  friend Tabber;
};



#endif
