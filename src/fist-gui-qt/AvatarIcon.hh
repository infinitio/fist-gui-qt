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
             QSize const& size,
             QWidget* parent = nullptr);
  virtual
  ~AvatarIcon() = default;

  void
  set_avatar(QPixmap const& pixmap);

protected:
  virtual
  void
  paintEvent(QPaintEvent*) override;

private:
  ELLE_ATTRIBUTE(QPixmap, pixmap);
  ELLE_ATTRIBUTE(QPixmap, mask);
  ELLE_ATTRIBUTE(int, border);
  ELLE_ATTRIBUTE(QRect, geometry);

private:
  Q_OBJECT;
};

#endif // !AVATARICON_HH
