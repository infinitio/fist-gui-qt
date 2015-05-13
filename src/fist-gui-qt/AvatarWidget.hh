#ifndef FIST_GUI_QT_AVATARWIDGET_HH
# define FIST_GUI_QT_AVATARWIDGET_HH

# include <map>

# include <QWidget>
# include <QByteArray>
# include <QPixmap>
# include <QPropertyAnimation>

# include <elle/attribute.hh>

# include <fist-gui-qt/utils.hh>

extern std::map<uint32_t, QPixmap> g_avatars;

class AvatarWidget:
  public QWidget
{
public:
  AvatarWidget(QWidget* parent = nullptr);
  AvatarWidget(QString const& picture, QWidget* parent = nullptr);
  AvatarWidget(QPixmap const& avatar, QWidget* parent = nullptr);
  virtual
  ~AvatarWidget() = default;

public:
  Q_OBJECT;
  Q_PROPERTY(QPixmap picture
             READ picture
             WRITE setPicture
             NOTIFY onPictureChanged);

public:
  QPixmap picture();
  void setPicture(QPixmap const& value);
Q_SIGNALS:
  void onPictureChanged();
private:
  QPixmap _picture;

  /*-------------.
  | Transactions |
  `-------------*/

public:
  static
  const int
  progress_update_interval = 1000; // in ms.

  Q_PROPERTY(float smooth_progress
             READ smooth_progress
             WRITE set_smooth_progress);

  Q_PROPERTY(float progress
             READ progress
             WRITE setProgress
             NOTIFY onProgressChanged);
  Q_PROPERTY(int transactionCount
             READ transactionCount
             WRITE setTransactionCount
             NOTIFY onTransactionCountChanged);

public Q_SLOTS:
  void setTransactionCount(int value);
  void setProgress(float value);
  void
  set_smooth_progress(float value);

Q_SIGNALS:
  void onTransactionCountChanged(int);
  void onProgressChanged(float);

private:
  uint32_t _uid;

  Q_PROPERTY_R(int, transaction_count, transactionCount);
  Q_PROPERTY_R(float, progress, progress);
  Q_PROPERTY_R(float, smooth_progress, smooth_progress);
  ELLE_ATTRIBUTE(QPropertyAnimation*, progress_animation);

  /*--------.
  | Drawing |
  `--------*/

protected:
  virtual
  void
  paintEvent(QPaintEvent*);
};

#endif
