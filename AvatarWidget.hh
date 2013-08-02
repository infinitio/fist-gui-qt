#ifndef AVATARWIDGET_HH
# define AVATARWIDGET_HH

# include <QWidget>

# include "utils.hh"

class AvatarWidget:
  public QWidget
{
  public:
    AvatarWidget();
    AvatarWidget(QString const& picture);

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

Q_SIGNALS:
  void onTransactionCountChanged(int);
  void onProgressChanged(float);

private:
  Q_PROPERTY_R(int, transaction_count, transactionCount);
  Q_PROPERTY_R(float, progress, progress);

/*--------.
| Drawing |
`--------*/

protected:
  virtual
  void
  paintEvent(QPaintEvent*);
};

#endif
