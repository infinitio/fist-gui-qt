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
  AvatarIcon(gap_State* state, uint32_t uid);

protected:
  virtual void paintEvent(QPaintEvent*) override;

public slots:
  void slot_netwManagerFinished(QNetworkReply *reply);

private:
  uint32_t _uid;

  QPixmap _pixmap;
  QPixmap _mask;
  QRect _geometry;

  Q_OBJECT;
};

#endif // !AVATARICON_HH
