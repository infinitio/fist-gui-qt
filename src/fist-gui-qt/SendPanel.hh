#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QWidget>

# include <unordered_map>

# include <surface/gap/gap.h>

# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/SendFooter.hh>
# include <fist-gui-qt/UserModel.hh>
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
public slots:
  void
  avatar_available(uint32_t uid);

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
  HorizontalSeparator* _file_part_seperator;
  ListWidget* _file_list;
  AddFileWidget* _file_adder;

/*-------------.
| SearchResult |
`-------------*/
  uint32_t _peer_id;

  // FreeSearch is an functor to replace standard unique_ptr default delete.
  QVector<uint32_t> _results;
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
