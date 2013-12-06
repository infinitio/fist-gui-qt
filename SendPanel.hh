#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QLineEdit>
# include <QPushButton>
# include <QWidget>
# include <QListWidget>
# include <QSet>

# include <unordered_map>

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
  add_file(QString const& path);

private Q_SLOTS:
  void
  remove_file(QString const& path);

private slots:
  void _send();
  void _search_changed(QString const& search);
  void _set_peer(uint32_t uid);
  void _pick_user();
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
  void choose_files();

private:
  gap_State* _state;
  SearchField* _search;
  ListWidget* _users;
  AddFileWidget* _file_adder;
  ListWidget* _file_list;
  uint32_t _peer_id;
  bool _ignore_search_result;

protected:
  void
  keyPressEvent(QKeyEvent* event) override;

/*-------.
| Layout |
`-------*/
private:
  std::unordered_map<uint32_t, UserModel> _user_models;

  QHash<QString, FileItem*> _files;

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
