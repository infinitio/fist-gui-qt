#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QUrl>
# include <QWidget>
# include <QFuture>
# include <QFutureWatcher>

# include <unordered_map>
# include <vector>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

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
signals:
  void
  drag_entered();

  void
  drag_left();

public Q_SLOTS:
  void
  add_file(QUrl const& path);

private Q_SLOTS:
  void
  remove_file(QUrl const& path);

private slots:
  void
  _send();

  void
  _search_changed(QString const& search);

  void
  _set_peer(uint32_t uid);

  void
  _pick_user();
public slots:
  void
  avatar_available(uint32_t uid);

/*------.
| Users |
`------*/
public:
  void
  _clean_results();

  void
  set_users(std::vector<uint32_t> const& users);

  void
  clearUsers();

protected slots:
  void
  _set_users();

Q_SIGNALS:
  void
  switch_signal();

  void
  choose_files();

/*--------------.
| Drag and drop |
`--------------*/
public:
  void
  dragEnterEvent(QDragEnterEvent *event) override;

  void
  dragLeaveEvent(QDragLeaveEvent *event) override;

  void
  dropEvent(QDropEvent *event) override;

private:
  ELLE_ATTRIBUTE(gap_State*, state);
  ELLE_ATTRIBUTE(SearchField*, search);
  ELLE_ATTRIBUTE(HorizontalSeparator*, users_part_separator);
  ELLE_ATTRIBUTE(ListWidget*, users);
  ELLE_ATTRIBUTE(HorizontalSeparator*, file_part_seperator);
  ELLE_ATTRIBUTE(ListWidget*, file_list);
  ELLE_ATTRIBUTE(HorizontalSeparator*, adder_part_seperator);
  ELLE_ATTRIBUTE(AddFileWidget*, file_adder);

/*-------------.
| SearchResult |
`-------------*/
  ELLE_ATTRIBUTE(uint32_t, peer_id);
  typedef QFuture<std::vector<uint32_t>> FutureSearchResult;
  ELLE_ATTRIBUTE(FutureSearchResult, search_future);
  ELLE_ATTRIBUTE(QFutureWatcher<std::vector<uint32_t>>, search_watcher);
  ELLE_ATTRIBUTE(QPixmap, magnifier);
  ELLE_ATTRIBUTE(QMovie*, loading_icon);
  ELLE_ATTRIBUTE(QVector<uint32_t>, results);
  ELLE_ATTRIBUTE(bool, ignore_search_result);

protected:
  void
  keyPressEvent(QKeyEvent* event) override;

/*-------.
| Layout |
`-------*/
private:
  std::unordered_map<uint32_t, UserModel> _user_models;
  QHash<QUrl, FileItem*> _files;

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

public:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
