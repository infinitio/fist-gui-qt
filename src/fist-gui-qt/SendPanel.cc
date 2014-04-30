#include <QDir>
#include <QFuture>
#include <QtConcurrentRun>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/TextListItem.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

namespace
{
  static const QRegExp email_checker(regexp::email,
                                     Qt::CaseInsensitive);
}

ELLE_LOG_COMPONENT("infinit.FIST.SendPanel");

SendPanel::SendPanel(gap_State* state):
  Super(new SendFooter),
  _state(state),
  _search(new SearchField(this)),
  _users_part_separator(new HorizontalSeparator(this)),
  _users(new ListWidget(this)),
  _file_part_seperator(new HorizontalSeparator(this)),
  _file_list(new ListWidget(this)),
  _adder_part_seperator(new HorizontalSeparator(this)),
  _file_adder(new AddFileWidget(this)),
  _peer_id(gap_null()),
  _search_future(),
  _search_watcher(),
  _magnifier(":/icons/search@2x.png"),
  _loading_icon(new QMovie(QString(":/icons/loading.gif"), QByteArray(), this)),
  _results(),
  _ignore_search_result(false)
{
  this->footer()->setParent(this);

  this->_users->setMaxRows(4);

  {
    this->_file_list->setMaxRows(3);
    auto palette = this->_file_list->palette();
    palette.setColor(QPalette::Window, view::send::file_adder::background);
    this->_file_list->setPalette(palette);
    this->_file_list->setAutoFillBackground(true);
  }

  connect(this->_search, SIGNAL(return_pressed()),
          this, SLOT(_pick_user()));

  this->_search->set_icon(this->_magnifier);

  connect(this->_file_adder, SIGNAL(clicked()),
          this, SIGNAL(choose_files()));

  connect(this->_file_adder->attach(), SIGNAL(released()),
          this, SIGNAL(choose_files()));

  connect(this->_file_adder, SIGNAL(file_dropped(QUrl const&)),
          this, SLOT(add_file(QUrl const&)));

  connect(this->footer()->send(), SIGNAL(clicked()),
          this, SLOT(_send()));

  connect(this->_search, SIGNAL(search_ready(QString const&)),
          this, SLOT(_search_changed(QString const&)));

  connect(this, SIGNAL(drag_entered()),
          this->_file_adder, SLOT(on_entered()));

  connect(this, SIGNAL(drag_left()),
          this->_file_adder, SLOT(on_left()));

  //XXX: Could be factored.
  this->_users_part_separator->hide();
  this->_adder_part_seperator->hide();

  connect(&this->_search_watcher, SIGNAL(finished()),
          this, SLOT(_set_users()));

  this->setAcceptDrops(true);
}

void
SendPanel::_search_changed(QString const& search)
{
  ELLE_TRACE_SCOPE("%s: search changed: %s", *this, search);

  if (this->_ignore_search_result)
  {
    ELLE_DEBUG("result ignored");
    this->_ignore_search_result = false;
    return;
  }

  QStringList res;

  if (this->_peer_id != gap_null())
  {
    ELLE_DEBUG("reset selection");
    this->_search->set_icon(this->_magnifier);
    this->_peer_id = gap_null();
  }

  if (search.size() != 0)
  {
    std::string text(search.toStdString());

    this->_search->set_icon(*this->_loading_icon);
    this->_search_future.cancel();
    this->_search_future = QtConcurrent::run(
      [&,search,text] {
        if (search.count('@') == 1 && email_checker.exactMatch(search))
          return std::vector<uint32_t>{gap_user_by_email(this->_state, text.c_str())};
        else
          return gap_users_search(this->_state, text.c_str());
      });

    this->_search_watcher.setFuture(this->_search_future);
  }
  else
  {
    this->_search_future = FutureSearchResult();
    this->clearUsers();
  }
}

/*------.
| Files |
`------*/

void
SendPanel::add_file(QUrl const& path)
{
  ELLE_TRACE_SCOPE("%s: add file: %s", *this, path);

  this->_search->setFocus();

  if (this->_files.contains(path))
    return;

  this->_adder_part_seperator->show();

  this->_files.insert(path, new FileItem(path));
  connect(this->_files[path], SIGNAL(remove(QUrl const&)),
          this, SLOT(remove_file(QUrl const&)));
  this->_file_list->add_widget(this->_files[path]);
}

void
SendPanel::remove_file(QUrl const& path)
{
  ELLE_TRACE_SCOPE("%s: remove file: %s", *this, path);

 auto it = this->_files.find(path);

  if (it != this->_files.end())
  {
    this->_file_list->remove_widget(it.value());
    this->_files.remove(path);
  }
  else
  {
    ELLE_DEBUG_SCOPE("no file deleted");
    for (auto const& file: this->_files.keys())
      ELLE_DEBUG("%s", file);
  }

  if (this->_files.empty())
    this->_adder_part_seperator->hide();
}

/*------.
| Users |
`------*/

void
SendPanel::_clean_results()
{
  this->_users->clearWidgets();
  this->_results.clear();
  this->_users_part_separator->hide();
  this->update();
}

void
SendPanel::_set_users()
{
  this->set_users(this->_search_watcher.result());
}

void
SendPanel::set_users(std::vector<uint32_t> const& users)
{
  ELLE_TRACE_SCOPE("%s: set users", *this);
  elle::SafeFinally restore_magnifier(
    [&] { this->_search->set_icon(this->_magnifier); });
  this->_clean_results();

  if (users.size() == 0)
  {
    this->_users->add_widget(
      new TextListItem("No result", 30, this), ListWidget::Position::Top);
  }
  else
  {
    for (auto uid: users)
    {
      this->_results.push_back(uid);
      if (this->_user_models.find(uid) == this->_user_models.end())
        this->_user_models.emplace(uid, UserModel(this->_state, uid));
      auto widget = new SearchResultWidget(this->_user_models.at(uid), this);
      connect(widget,
              SIGNAL(clicked_signal(uint32_t)),
              this,
              SLOT(_set_peer(uint32_t)));
      this->_users->add_widget(widget, ListWidget::Position::Top);
    }
  }
  this->_users_part_separator->show();
}


void
SendPanel::clearUsers()
{
  ELLE_TRACE_SCOPE("%s: clear user list", *this);
  this->_clean_results();
  this->_users_part_separator->hide();
}

/*------.
| Slots |
`------*/
void
SendPanel::_set_peer(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: set peer: %s", *this, this->_user_models.at(uid));
  this->clearUsers();
  this->_ignore_search_result = true;
  this->_search->set_text(this->_user_models.at(uid).fullname());
  this->_search->set_icon(this->_user_models.at(uid).avatar());
  this->_peer_id = uid;
}

void
SendPanel::_pick_user()
{
  ELLE_TRACE_SCOPE("%s: pick user", *this);
  if (this->_peer_id == gap_null())
  {
    if (this->_results.empty())
      return;
    auto uid = this->_results.last();
    this->_set_peer(uid);
  }
  else
  {
    this->_send();
  }
}

void
SendPanel::_send()
{
  ELLE_TRACE_SCOPE("%s: send", *this);

  static QRegExp email_checker(regexp::email,
                               Qt::CaseInsensitive);

  if (this->_peer_id == gap_null() &&
      !email_checker.exactMatch(this->_search->text()))
  {
    ELLE_DEBUG("peer is not set");
    this->_search->setFocus();
    return;
  }

  if (this->_files.empty())
  {
    ELLE_DEBUG("file list is empty");
    this->_file_adder->pulse();
    return;
  }

  char** filenames;
  if ((filenames = (char**)malloc((this->_files.size() + 1) * sizeof(char*)))
      == nullptr)
  {
    ELLE_ERR("unable to allocate file list");
  }

  for (int i = 0; i < this->_files.size(); i++)
  {
    auto filepath =
      QDir::toNativeSeparators(this->_files.keys().at(i).toLocalFile()).toStdString();

    ELLE_DEBUG("file to add: %s", filepath);

    if ((filenames[i] = (char*)malloc((filepath.size() + 1))) == nullptr)
    {
      ELLE_ERR("unable to allocate file name");
    }

    strcpy(filenames[i], filepath.c_str());
  }

  filenames[this->_files.size()] = nullptr;

  if (this->_peer_id != gap_null())
  {
    ELLE_TRACE_SCOPE("send files to %s", this->_user_models.at(this->_peer_id));
    gap_send_files(_state, this->_peer_id, filenames, "");
  }
  else
  {
    ELLE_TRACE_SCOPE("send files to %s", this->_search->text());
    gap_send_files_by_email(_state,
                            this->_search->text().toStdString().c_str(),
                            filenames,
                            "");
  }

  auto** cpy = filenames;
  while (*cpy != nullptr)
  {
    ::free((void*) *cpy);
    ++cpy;
  }
  ::free((void*) filenames);

  emit switch_signal();
}

void
SendPanel::avatar_available(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: user (%s) avatar available", *this, uid);
  auto it = this->_user_models.find(uid);
  if (it != this->_user_models.end())
  {
    it->second.avatar_available();
  }

  this->_users->reload();
}

void
SendPanel::keyPressEvent(QKeyEvent* event)
{
  ELLE_DEBUG_SCOPE("key pressed: %s", event->key());
  if (event->key() == Qt::Key_Escape)
    emit switch_signal();
  if (!event->text().isEmpty())
    this->_search->insert_text(event->text());
}

/*------------.
| Show & Hide |
`------------*/
void
SendPanel::on_show()
{
  emit set_background_color(Qt::white);
  this->_search->setFocus();
  this->update();
}

void
SendPanel::on_hide()
{
  emit set_background_color(view::background);
  this->_search->clear();
  this->_file_list->clearWidgets();
  this->_files.clear();
  this->_results.clear();
  this->_peer_id = gap_null();
  this->_adder_part_seperator->hide();
  this->_search->set_icon(this->_magnifier);
}

/*-------.
| Footer |
`-------*/
SendFooter*
SendPanel::footer()
{
  return static_cast<SendFooter*>(this->_footer);
}

void
SendPanel::print(std::ostream& stream) const
{
  stream << "SendPanel";
}

void
SendPanel::dragEnterEvent(QDragEnterEvent *event)
{
  ELLE_TRACE_SCOPE("%s: drag entered", *this);

  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        emit drag_entered();
        return;
      }
}

void
SendPanel::dragLeaveEvent(QDragLeaveEvent *event)
{
  ELLE_TRACE_SCOPE("%s: drag left", *this);
  emit drag_left();
}

void
SendPanel::dropEvent(QDropEvent *event)
{
  ELLE_TRACE_SCOPE("%s: drop", *this);

  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        this->add_file(url);
      }
}
