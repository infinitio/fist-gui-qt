#ifndef ICONBUTTON_HH
# define ICONBUTTON_HH

# include <memory>

# include <QPainter>
# include <QPushButton>

# include <fist-gui-qt/utils.hh>

# include <functional>

# include <elle/attribute.hh>

class IconButton:
  public QPushButton
{
  Q_OBJECT;

  typedef std::function<void ()> Callback;
public:
  IconButton(QString const& resource,
             QWidget* parent = nullptr,
             QSize const& size = QSize(),
             Callback const& = {});

public:
  void
  enable();

  void
  disable();

protected:
  virtual
  QSize
  sizeHint() const override;

  virtual
  void
  paintEvent(QPaintEvent*) override;

public:
  void
  enterEvent(QEvent*) override;

  void
  leaveEvent(QEvent*) override;

private slots:
  virtual
  void
  _clicked();

public slots:
  void
  set_pixmap(QString const& resource,
             QSize const& size = QSize(),
             bool hover = false);

private:
  ELLE_ATTRIBUTE(QString, resource);
  ELLE_ATTRIBUTE(QSize, size);
  QPixmap _original;
  Callback _callback;
};

#endif
