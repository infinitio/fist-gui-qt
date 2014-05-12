#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QUrl>
# include <QWidget>
# include <QFuture>
# include <QFutureWatcher>

# include <unordered_map>
# include <vector>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/SendFooter.hh>
# include <fist-gui-qt/UserModel.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/State.hh>

class SendPanel:
  public Panel
{
public:
  class Selected
  {
  public:
    Selected(QString const& email)
      : _id(gap_null())
      , _email(email)
    {}

    Selected(uint32_t id)
      : _id(id)
      , _email()
    {}

    ELLE_ATTRIBUTE_R(uint32_t, id);
    ELLE_ATTRIBUTE_R(QString, email);
  };

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
  SendPanel(fist::State& state);

/*------.
| Files |
`------*/
signals:
  void
  drag_entered();

  void
  drag_left();

  void
  shown();

  void
  peer_found();

  void
  file_added();

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
  _search_ready(QString const& search);

  void
  _search_changed(QString const& search);

  void
  _add_peer(uint32_t uid);

  void
  _remove_peer(uint32_t uid);

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
  set_users(fist::State::Users const&, bool local);

  void
  clearUsers();

  bool
  peer_valid() const;

protected slots:
  void
  _set_users();


  // XXX
  void
  _expand_files();

  void
  _shrink_files();

Q_SIGNALS:
  void
  switch_signal();

  void
  choose_files();

  void
  sent();

  void
  canceled();

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
  ELLE_ATTRIBUTE(fist::State&, state);
  ELLE_ATTRIBUTE_R(SearchField*, search);
  ELLE_ATTRIBUTE(HorizontalSeparator*, users_part_separator);
  ELLE_ATTRIBUTE(ListWidget*, users);
  ELLE_ATTRIBUTE(HorizontalSeparator*, file_part_seperator);
  ELLE_ATTRIBUTE_R(AddFileWidget*, file_adder);
  ELLE_ATTRIBUTE(HorizontalSeparator*, adder_part_seperator);
  ELLE_ATTRIBUTE(ListWidget*, file_list);

/*-------------.
| SearchResult |
`-------------*/
  typedef std::unordered_map<uint32_t, SearchResultWidget*> Results;
  ELLE_ATTRIBUTE(Results, results);
  typedef std::unordered_set<uint32_t> Recipients;
  ELLE_ATTRIBUTE(Recipients, recipients);
  ELLE_ATTRIBUTE(QPixmap, magnifier);
  ELLE_ATTRIBUTE(QMovie*, loading_icon);

protected:
  void
  keyPressEvent(QKeyEvent* event) override;

/*-------.
| Layout |
`-------*/
private:
  std::unordered_map<uint32_t, std::unique_ptr<UserModel>> _user_models;
  typedef QHash<QUrl, FileItem*> Files;
  ELLE_ATTRIBUTE_R(Files, files);

  void
  on_show() override;

  void
  on_hide() override;

  void
  focusInEvent(QFocusEvent* event) override;

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

# include <fist-gui-qt/SendPanel.hxx>

#endif
