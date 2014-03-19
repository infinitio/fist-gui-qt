#ifndef USERWIDGET_HH
# define USERWIDGET_HH

# include <memory>

# include <QWidget>
# include <QPushButton>
# include <QTimer>
# include <QLabel>

# include <surface/gap/gap.h>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/IconButton.hh>

class UserWidget:
  public ListItem
{
public:
  UserWidget(UserModel const& model,
             QWidget* parent = nullptr);
  ~UserWidget();

/*-----------.
| Properties |
`-----------*/

public:
  Q_PROPERTY(float progress
             READ progress
             WRITE setProgress
             NOTIFY onProgressChanged);

public:
  float progress() const;

public Q_SLOTS:
  void setProgress(float value);

Q_SIGNALS:
  void onProgressChanged(float);

  void
  clicked(uint32_t);
/*-------.
| Layout |
`-------*/

public:
  virtual
  QSize
  sizeHint() const override;

  virtual
  QSize
  minimumSizeHint() const override;

  virtual
  void
  trigger();

public:
  void
  _update() override;

public slots:
  void update_progress();
  void update_status();

private:
  UserModel const& _user;
  AvatarWidget* _peer_avatar;
  QLabel* _peer_status;
  QLayout* _layout;
  QLabel* _notification;
  QLabel* _mtime;
  QLabel* _status;
  QTimer* _timer;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;

  Q_OBJECT;
};

#endif
