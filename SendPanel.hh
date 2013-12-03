#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QLineEdit>
# include <QPushButton>
# include <QWidget>
# include <QListWidget>

# include <unordered_map>
# include <set>

# include <surface/gap/gap.h>

# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/SendFooter.hh>
# include <fist-gui-qt/SmoothLayout.hh>
# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/UserWidget.hh>
# include <fist-gui-qt/fwd.hh>

class SendPanel:
  public Panel
{
/*------.
| Types |
`------*/
public:
  typedef SendPanel Self;
  typedef Panel Super;

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
  void _search_changed(QString const& search);
/*------.
| Users |
`------*/
public:
  void
  setUsers(uint32_t* uids = nullptr);
  void
  clearUsers();

Q_SIGNALS:
  void switch_signal();
private:
  ListWidget* _users;
  SearchField* _search;
  QPushButton* _send;
  gap_State* _state;

protected:
  void
  keyPressEvent(QKeyEvent* event) override;

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;
private:
  std::unordered_map<uint32_t, UserModel> _user_models;
  ListWidget* _file_list;
  std::set<QString> _files;

  void
  on_show() override;

  void
  on_hide() override;

/*-------.
| Footer |
`-------*/
public:
  SendFooter*
  footer() override;

private:
  Q_OBJECT;
};

#endif
