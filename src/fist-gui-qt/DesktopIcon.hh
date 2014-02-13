#ifndef DESKTOPICON_HH
# define DESKTOPICON_HH

# include <boost/logic/tribool.hpp>

# include <QMainWindow>
# include <QPropertyAnimation>
# include <QTimer>

class DesktopIcon:
  public QMainWindow
{
public:
  DesktopIcon(QWidget* parent = nullptr);

private Q_SLOTS:
  void
  expand();

  void
  collapse();

private:
  boost::logic::tribool
  _expanding;

private Q_SLOTS:
  void
  _reset_expanding();

public:
  Q_PROPERTY(int posx READ posx WRITE set_posx NOTIFY posx_changed);
  int posx()
  {
    return this->_posx;
  }

  void set_posx(int value)
  {
    this->_posx = value;
    this->move(this->_posx, this->y());
    emit this->posx_changed();
  }

Q_SIGNALS:
  void posx_changed();
private:
  int _posx;

public:
  void
  enterEvent(QEvent* e) override;

  void
  leaveEvent(QEvent* e) override;

public:
  QSize
  sizeHint() const override;

private:
  QPropertyAnimation*
  _animator;

  QTimer*
  _timer;

  Q_OBJECT
};

#endif
