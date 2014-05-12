#include <iostream>

#include <QDir>
#include <QFuture>
#include <QtConcurrentRun>

#include <elle/assert.hh>
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

SendPanel::SendPanel(fist::State& state):
  Super(new SendFooter),
  _state(state),
  _search(new SearchField(this)),
  _users_part_separator(new HorizontalSeparator(this)),
  _users(new ListWidget(this)),
  _file_part_seperator(new HorizontalSeparator(this)),
  _file_adder(new AddFileWidget(new ListWidget, this)),
  _file_list(static_cast<ListWidget*>(_file_adder->body())),
  _adder_part_seperator(new HorizontalSeparator(this)),
  _magnifier(":/icons/search@2x.png"),
  _loading_icon(new QMovie(QString(":/icons/loading.gif"), QByteArray(), this)),
  _results()
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
  this->_file_list->setStyleSheet("color:green;");

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

  connect(this->footer()->back(), SIGNAL(clicked()),
          this, SIGNAL(canceled()));

  connect(this->_search, SIGNAL(search_ready(QString const&)),
          this, SLOT(_search_ready(QString const&)));

  connect(this->_search, SIGNAL(search_changed(QString const&)),
          this, SLOT(_search_changed(QString const&)));

  connect(this, SIGNAL(drag_entered()),
          this->_file_adder, SLOT(on_entered()));

  connect(this, SIGNAL(drag_left()),
          this->_file_adder, SLOT(on_left()));

  connect(this, SIGNAL(sent()),
          this, SIGNAL(switch_signal()));

  connect(this, SIGNAL(canceled()),
          this, SIGNAL(switch_signal()));

  // connect(this->_file_adder->expanser(), SIGNAL(pressed()),
  //         this, SLOT(_shrink_files()));

  // connect(this->_file_adder->expanser(), SIGNAL(released()),
  //         this, SLOT(_expand_files()));

  //XXX: Could be factored.
  this->_users_part_separator->hide();
  this->_adder_part_seperator->hide();

  connect(&this->_state, SIGNAL(results_ready()),
          this, SLOT(_set_users()));

  this->_file_adder->expanser()->hide();
  this->setAcceptDrops(true);

  this->_search_changed("");
}

void
SendPanel::_search_changed(QString const& search)
{
  this->_clean_results();

  if (search.isEmpty())
    this->set_users(this->_state.swaggers(), true);
  else
    this->set_users(this->_state.swaggers(search), true);
}

void
SendPanel::_search_ready(QString const& search)
{
  ELLE_TRACE_SCOPE("%s: search changed: %s", *this, search);
  // if (this->_peer_id != gap_null())
  // {
  //   ELLE_DEBUG("reset selection");
  //   this->_search->set_icon(this->_magnifier);
  //   this->_peer_id = gap_null();
  // }

  QString trimmed_search = search.trimmed();
  if (trimmed_search.size() != 0)
  {
    this->_search->set_icon(*this->_loading_icon);
    this->_state.search(trimmed_search);
  }
  else
  {
    ELLE_DEBUG("reset search future")
    {
      // this->clearUsers();
    }
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

  this->_file_adder->expanser()->show();
  emit file_added();

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
  {
    this->_file_adder->expanser()->hide();
    this->_adder_part_seperator->hide();
  }
}

/*------.
| Users |
`------*/

void
SendPanel::_clean_results()
{
  this->_results.clear();
  this->_users->clearWidgets();
  this->_users_part_separator->hide();
  this->update();
}

void
SendPanel::_set_users()
{
  ELLE_TRACE_SCOPE("got result from future");

  elle::SafeFinally restore_magnifier(
    [&] { this->_search->set_icon(this->_magnifier); });

  this->set_users(this->_state.results(), false);
}

void
SendPanel::set_users(fist::State::Users const& users, bool local)
{
  ELLE_TRACE_SCOPE("%s: set users", *this);
  for (UserModel* model: users)
  {
    if (this->_results.find(model->id()) == this->_results.end())
    {
      bool picked = this->_recipients.find(model->id()) != this->_recipients.end();
      auto widget = new SearchResultWidget(*model, picked, this);
      connect(widget,
              SIGNAL(selected(uint32_t)),
              this,
              SLOT(_add_peer(uint32_t)));
      connect(widget,
              SIGNAL(unselected(uint32_t)),
              this,
              SLOT(_remove_peer(uint32_t)));
      this->_users->add_widget(widget, ListWidget::Position::Bottom);
      this->_results[model->id()] = widget;
    }
  }

  if (this->_users->widgets().isEmpty() &&
      !local &&
      !email_checker.exactMatch(this->_search->search_field()->text()))
  {
    this->_users->add_widget(
      new TextListItem("<b>No result</b><br />Send to an email address instead",
                       60, this),
      ListWidget::Position::Top);
  }

  if (!this->_users->widgets().isEmpty())
    this->_users_part_separator->show();
  else
    emit peer_found();
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
SendPanel::_add_peer(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: add peer: %s", *this, uid);
  this->_recipients.insert(uid);
  emit peer_found();
}

void
SendPanel::_remove_peer(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: remove peer: %s", *this, uid);
  this->_recipients.erase(uid);
}

void
SendPanel::_pick_user()
{
  ELLE_TRACE_SCOPE("%s: pick user", *this);
  if (this->_recipients.empty())
  {
    // auto uid = this->_results.last();
    // this->_set_peer(uid);
  }
  else
  {
    // ELLE_DEBUG("try to send to %s", this->_peer_id);
    this->_send();
  }
}

bool
SendPanel::peer_valid() const
{
  static QRegExp email_checker(regexp::email,
                               Qt::CaseInsensitive);

  return //this->_peer_id != gap_null() ||
    email_checker.exactMatch(this->_search->text());
}

void
SendPanel::_send()
{
  ELLE_TRACE_SCOPE("%s: send", *this);

  if (!this->peer_valid())
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

  for (auto const& recipient: this->_recipients)
  {
    if (recipient != gap_null())
    {
      // ELLE_TRACE_SCOPE("send files to %s", *this->_user_models.at(this->_peer_id));
      gap_send_files(this->_state.state(), recipient, filenames, "");
    }
    // else if (!recipient.email().isNull())
    // {
    //   ELLE_TRACE_SCOPE("send files to %s", this->_search->text());
    //   gap_send_files_by_email(this->_state.state(),
    //                           recipient.email().toStdString().c_str(),
    //                           filenames,
    //                           "");
    // }
  }

  auto** cpy = filenames;
  while (*cpy != nullptr)
  {
    ::free((void*) *cpy);
    ++cpy;
  }
  ::free((void*) filenames);

  emit sent();
}

void
SendPanel::avatar_available(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: user (%s) avatar available", *this, uid);
  auto it = this->_user_models.find(uid);
  if (it != this->_user_models.end())
  {
    it->second->avatar_available();
  }
}

void
SendPanel::keyPressEvent(QKeyEvent* event)
{
  ELLE_DEBUG_SCOPE("key pressed: %s", event->key());
  if (event->key() == Qt::Key_Escape)
    emit canceled();
  else if (event->key() == Qt::Key_Return)
    this->_pick_user();
  else if (this->_search->search_field() != nullptr)
  {
    this->_search->search_field()->setFocus();
    static_cast<QObject*>(this->_search->search_field())->event(event);
  }
}

/*------------.
| Show & Hide |
`------------*/
void
SendPanel::on_show()
{
  emit set_background_color(Qt::white);
  emit shown();
  this->_search_changed("");
  this->_search->setFocus();
  this->update();
}

void
SendPanel::on_hide()
{
  emit set_background_color(view::background);

  this->_clean_results();
  this->_file_adder->expanser()->hide();
  this->_search->clear();
  this->_file_list->clearWidgets();
  this->_files.clear();
  this->_recipients.clear();
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

void
SendPanel::focusInEvent(QFocusEvent* event)
{
  this->_search->setFocus();
}

// XXX
void
SendPanel::_expand_files()
{
  std::cerr << "bite" << std::endl;
  //  this->_file_list->setMaxRows(3);
}

void
SendPanel::_shrink_files()
{
  std::cerr << "boite" << std::endl;
  // this->_file_list->setMaxRows(0);
}
