#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <unordered_map>

# include <QLineEdit>
# include <QPushButton>
# include <QWidget>

# include <surface/gap/gap.h>

# include <fist-gui-qt/SmoothLayout.hh>
# include <fist-gui-qt/UserWidget.hh>
# include <fist-gui-qt/AvatarIcon.hh>
# include <fist-gui-qt/fwd.hh>

class SendPanel:
  public SmoothLayout
{
/*------.
| Types |
`------*/
public:
  typedef SendPanel Self;
  typedef SmoothLayout Super;

/*-------------.
| Construction |
`-------------*/
public:
  SendPanel(gap_State* state);

/*------.
| Files |
`------*/
public:
  void
  addFile(const QString& path);

public slots:
  void send(uint32_t uid = 0);

/*------.
| Users |
`------*/
public:
  void
  setUsers(QStringList const& users, uint32_t* uids = nullptr);
  void
  clearUsers();
Q_SIGNALS:
  void onSearchChanged(QString const& search);
  void switch_signal();
private:
  QLineEdit* _search;
  QPushButton* _send;
  ListWidget* _users;
  std::unordered_map<uint32_t, UserModel> _user_models;
  gap_State* _state;

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;
private:
  std::string _file_path;

private:
  Q_OBJECT;
};

#endif
