#ifndef FIST_GUI_QT_SMOOTHSCROLLBAR_HH
# define FIST_GUI_QT_SMOOTHSCROLLBAR_HH

# include <QPropertyAnimation>
# include <QWidget>

class SmoothScrollBar:
  public QWidget
{
  public:
    Q_OBJECT;
  public:
#define PROPERTY(Type, Name)                                          \
  private:                                                            \
    Type _##Name;                                                     \

#define PROPERTY_r(Type, Read)                                        \
  public:                                                             \
  Type const&                                                         \
  Read() const                                                        \

#define PROPERTY_R(Type, Name, Read)                                  \
  PROPERTY_r(Type, Read)                                              \
  {                                                                   \
    return this->_##Name;                                             \
  }                                                                   \

#define PROPERTY_w(Type, Write)                                       \
  public:                                                             \
  void                                                                \
  Write(Type const& value)                                            \

#define PROPERTY_W(Type, Name, Write, Notify)                         \
  PROPERTY_w(Type, Write)                                             \
  {                                                                   \
    if (this->_##Name != value)                                       \
    {                                                                 \
      this->_##Name = value;                                          \
      Notify;                                                         \
    }                                                                 \
  }                                                                   \

#define PROPERTY_RWN(Type, Name, Read, Write, Notify)                 \
  PROPERTY(Type, Name);                                               \
  PROPERTY_R(Type, Name, Read);                                       \
  PROPERTY_W(Type, Name, Write, Notify);                              \

#define PROPERTY_RW(Type, Name, Read, Write)                          \
  PROPERTY_RWN(Type, Name, Read, Write,)                              \

#define PROPERTY_Rw(Type, Name, Read, Write)                          \
  PROPERTY(Type, Name);                                               \
  PROPERTY_R(Type, Name, Read);                                       \
  PROPERTY_w(Type, Write);                                            \

  Q_PROPERTY(int minimum READ minimum WRITE setMinimum);
  PROPERTY_RWN(int, minimum, minimum, setMinimum,
               QPropertyAnimation* animation =
               new QPropertyAnimation(this, "displayMinimum");
               animation->setDuration(200);
               animation->setEndValue(value);
               animation->start();
    );
  Q_PROPERTY(int maximum READ maximum WRITE setMaximum);
  PROPERTY_Rw(int, maximum, maximum, setMaximum);

  Q_PROPERTY(int pageSize READ pageSize WRITE setPageSize);
  PROPERTY_RWN(int, pageSize, pageSize, setPageSize,
               QPropertyAnimation* animation =
               new QPropertyAnimation(this, "displayPageSize");
               animation->setDuration(200);
               animation->setEndValue(value);
               animation->start();
    );

  Q_PROPERTY(int displayMinimum READ displayMinimum WRITE setDisplayMinimum);
  PROPERTY_RWN(int, displayMinimum, displayMinimum, setDisplayMinimum,
              this->update());
  Q_PROPERTY(int displayMaximum READ displayMaximum WRITE setDisplayMaximum);
  PROPERTY_RWN(int, displayMaximum, displayMaximum, setDisplayMaximum,
              this->update());
  Q_PROPERTY(int displayPageSize READ displayPageSize WRITE setDisplayPageSize);
  PROPERTY_RWN(int, displayPageSize, displayPageSize, setDisplayPageSize,
              this->update());
  Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged);
  PROPERTY_RWN(int, value, value, setValue,
              this->update();
              Q_EMIT valueChanged(value);
    );
private:
  int _value_target;
  QPropertyAnimation* _value_animation;
private:
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity);
  PROPERTY_RWN(float, opacity, opacity, setOpacity,
              this->update();
    );
  QPropertyAnimation* _opacity_animation;

private Q_SLOTS:
  void
  fade();
Q_SIGNALS:
  void valueChanged(int);

public:
  SmoothScrollBar(QWidget* parent);

  virtual
  QSize
  sizeHint() const override;

protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;
  virtual
  void
  wheelEvent(QWheelEvent*) override;
  void
  showEvent(QShowEvent*) override;

private:
  void
  _scroll(int delta);

  void
  _scroll_to(int destination, int speed = 600);
};

#endif
