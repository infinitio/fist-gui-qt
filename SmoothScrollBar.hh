#ifndef SMOOTHSCROLLBAR_HH
# define SMOOTHSCROLLBAR_HH

# include <QPropertyAnimation>
# include <QWidget>

class SmoothScrollBar:
  public QWidget
{
  public:
    Q_OBJECT;
  public:

#define PROPERTY_RW(Type, Name, Read, Write, Notify)    \
  public:                                               \
    Type                                                \
    Read() const                                        \
    {                                                   \
      return this->_##Name;                             \
    }                                                   \
                                                        \
    void                                                \
    Write(Type value)                                   \
    {                                                   \
      if (this->_##Name != value)                       \
      {                                                 \
        this->_##Name = value;                          \
        Notify;                                         \
      }                                                 \
    }                                                   \
                                                        \
  private:                                              \
    Type _##Name;                                       \

  Q_PROPERTY(int minimum READ minimum WRITE setMinimum);
  PROPERTY_RW(int, minimum, minimum, setMinimum,
              QPropertyAnimation* animation =
              new QPropertyAnimation(this, "displayMinimum");
              animation->setDuration(200);
              animation->setEndValue(value);
              animation->start();
    );
  Q_PROPERTY(int maximum READ maximum WRITE setMaximum);
  PROPERTY_RW(int, maximum, maximum, setMaximum,
              QPropertyAnimation* animation =
              new QPropertyAnimation(this, "displayMaximum");
              animation->setDuration(200);
              animation->setEndValue(value);
              animation->start();
    );
  Q_PROPERTY(int pageSize READ pageSize WRITE setPageSize);
  PROPERTY_RW(int, pageSize, pageSize, setPageSize,
              QPropertyAnimation* animation =
              new QPropertyAnimation(this, "displayPageSize");
              animation->setDuration(200);
              animation->setEndValue(value);
              animation->start();
    );

  Q_PROPERTY(int displayMinimum READ displayMinimum WRITE setDisplayMinimum);
  PROPERTY_RW(int, displayMinimum, displayMinimum, setDisplayMinimum,
              this->repaint());
  Q_PROPERTY(int displayMaximum READ displayMaximum WRITE setDisplayMaximum);
  PROPERTY_RW(int, displayMaximum, displayMaximum, setDisplayMaximum,
              this->repaint());
  Q_PROPERTY(int displayPageSize READ displayPageSize WRITE setDisplayPageSize);
  PROPERTY_RW(int, displayPageSize, displayPageSize, setDisplayPageSize,
              this->repaint());
  Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged);
  PROPERTY_RW(int, value, value, setValue,
              this->repaint();
              Q_EMIT valueChanged(value);
    );
private:
  int _value_target;
  QPropertyAnimation* _value_animation;
private:
  Q_PROPERTY(float opacity READ opacity WRITE setOpacity);
  PROPERTY_RW(float, opacity, opacity, setOpacity,
              this->repaint();
    );
  QPropertyAnimation* _opacity_animation;

  Q_PROPERTY(int step READ step WRITE setStep);
  PROPERTY_RW(int, step, step, setStep, );

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
private:
  void
  _scroll(bool up);
};

#endif
