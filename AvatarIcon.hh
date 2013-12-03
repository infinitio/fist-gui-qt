#ifndef AVATARICON_HH
# define AVATARICON_HH

# include <fist-gui-qt/AvatarWidget.hh>

# include <QPainter>

# include <surface/gap/gap.h>

class AvatarIcon:
  public QWidget
{
public:
  AvatarIcon(QPixmap const& pixmap);

protected:
  virtual void paintEvent(QPaintEvent*) override;

private:
  uint32_t _uid;

  QPixmap _pixmap;
  QPixmap _mask;
  QRect _geometry;

  Q_OBJECT;
};

#endif // !AVATARICON_HH
