#ifndef FIST_GUI_QT_AVATARICON_HH
# define FIST_GUI_QT_AVATARICON_HH

# include <fist-gui-qt/AvatarWidget.hh>

# include <QPainter>
# include <QSize>

class AvatarIcon:
  public QWidget
{
public:
  AvatarIcon(QPixmap const& pixmap,
             QSize const& size = QSize(35, 35));
  virtual
  ~AvatarIcon() = default;

  void
  set_avatar(QPixmap const& pixmap);

protected:
  virtual void paintEvent(QPaintEvent*) override;

private:
  uint32_t _uid;

  QPixmap _pixmap;
  QPixmap _mask;
  int _border;
  QRect _geometry;

  Q_OBJECT;
};

#endif // !AVATARICON_HH
