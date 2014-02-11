#include <iostream>

#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/SearchResultWidget.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

SendPanel::SendPanel(gap_State* state):
  Super(new SendFooter),
  _state(state),
  _search(new SearchField(this)),
  _users(new ListWidget(this)),
  _file_part_seperator(new HorizontalSeparator(this)),
  _file_list(new ListWidget(this)),
  _file_adder(new AddFileWidget(this)),
  _peer_id(gap_null()),
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

  connect(this->_search, SIGNAL(returnPressed()),
          this, SLOT(_pick_user()));

  this->_search->setIcon(QPixmap(":/icons/search@2x.png"));

  connect(this->_file_adder->attach(), SIGNAL(released()),
          this, SIGNAL(choose_files()));

  connect(this->footer()->send(), SIGNAL(clicked()),
          this, SLOT(_send()));

  connect(this->_search, SIGNAL(search_ready(QString const&)),
          this, SLOT(_search_changed(QString const&)));
}

void
SendPanel::_search_changed(QString const& search)
{
  if (this->_ignore_search_result)
  {
    this->_ignore_search_result = false;
    return;
  }

  QStringList res;

  if (this->_peer_id != gap_null())
  {
    this->_search->setIcon(QPixmap(":/icons/search@2x.png"));
    this->_peer_id = gap_null();
  }

  if (search.size() != 0)
  {
    std::string text(search.toStdString());
    this->setUsers(gap_search_users(_state, text.c_str()));
  }
  else
    this->clearUsers();
}

/*------.
| Files |
`------*/

void
SendPanel::add_file(QString const& path)
{
  if (this->_files.contains(path))
    return;

  this->_files.insert(path, new FileItem(path));
  connect(this->_files[path], SIGNAL(remove(QString const&)),
          this, SLOT(remove_file(QString const&)));
  this->_file_list->add_widget(this->_files[path]);
}

void
SendPanel::remove_file(QString const& path)
{
  auto it = this->_files.find(path);

  if (it != this->_files.end())
  {
    this->_file_list->remove_widget(it.value());
    this->_files.remove(path);
  }
}

/*------.
| Users |
`------*/

void
SendPanel::setUsers(uint32_t* uids)
{
  this->_users->clearWidgets();
  this->_results.clear();

  if (uids == nullptr)
    return;

  uint32_t* uidscopy = uids;
  while (*uidscopy != gap_null())
  {
    auto uid = *uidscopy;
    this->_results.push_back(uid);
    if (this->_user_models.find(uid) == this->_user_models.end())
      this->_user_models.emplace(uid, UserModel(this->_state, uid));
    auto widget = new SearchResultWidget(this->_user_models.at(uid), this);
    connect(widget,
            SIGNAL(clicked_signal(uint32_t)),
            this,
            SLOT(_set_peer(uint32_t)));
    this->_users->add_widget(widget, ListWidget::Position::Top);

    ++uidscopy;
  }
}

void
SendPanel::clearUsers()
{
  this->setUsers(nullptr);
}

/*------.
| Slots |
`------*/
void
SendPanel::_set_peer(uint32_t uid)
{
  this->_users->clearWidgets();
  this->_ignore_search_result = true;
  this->_search->setText(this->_user_models.at(uid).fullname());
  this->_search->setIcon(this->_user_models.at(uid).avatar());
  this->_peer_id = uid;
}

void
SendPanel::_pick_user()
{
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
  static QRegExp email_checker(regexp::email,
                               Qt::CaseInsensitive);

  if (this->_peer_id == gap_null() &&
      !email_checker.exactMatch(this->_search->text()))
  {
    this->_search->setFocus();
    return;
  }

  if (this->_files.empty())
  {
    this->_file_adder->pulse();
    return;
  }

  char** filenames;
  if ((filenames = (char**)malloc((this->_files.size() + 1) * sizeof(char*)))
      == nullptr)
  {
    std::cerr << "error: unable to allocate" << std::endl;
  }

  for (int i = 0; i < this->_files.size(); i++)
  {
    if ((filenames[i] = (char*)malloc((this->_files.keys().at(i).size() + 1)))
        == nullptr)
    {
      std::cerr << "error: unable to allocate" << std::endl;
    }

    strcpy(filenames[i], this->_files.keys().at(i).toStdString().c_str());
  }
  filenames[this->_files.size()] = nullptr;

  if (this->_peer_id != gap_null())
    gap_send_files(_state, this->_peer_id, filenames, "");
  else
    gap_send_files_by_email(_state,
                            this->_search->text().toStdString().c_str(),
                            filenames,
                            "");

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
  if (event->key() == Qt::Key_Escape)
    emit switch_signal();
}

/*------------.
| Show & Hide |
`------------*/
void
SendPanel::on_show()
{
  emit set_background_color(Qt::white);
  this->_search->setFocus();
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
}

/*-------.
| Footer |
`-------*/
SendFooter*
SendPanel::footer()
{
  return static_cast<SendFooter*>(this->_footer);
}
