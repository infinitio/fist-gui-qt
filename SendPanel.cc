#include <QEvent>
#include <QLineEdit>
#include <QPainter>
#include <QTextEdit>
#include <QGridLayout>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/AddFileWidget.hh>
#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/SearchField.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

namespace
{
  struct Separator:
    public QFrame
  {
    Separator()
    {
      this->setFrameShape(QFrame::HLine);
      this->setFrameShadow(QFrame::Sunken);

      this->setFixedHeight(5);
      this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
  };
}

SendPanel::SendPanel(gap_State* state):
  Super(new SendFooter),
  _users(new ListWidget
),
  _search(new SearchField(this, &this->_users)),
  _file_adder(new AddFileWidget),
  _state(state),
  _file_list(new ListWidget)
{
  this->_users->set_mate(this->_search);
  this->_users->setMaxRows(5);
  this->_file_list->setMaxRows(4);

  this->footer()->send()->disable();
  this->_search->setIcon(QPixmap(":/icons/magnifier.png"));

  {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->_search);
    layout->addWidget(this->_users);
    layout->addWidget(new Separator);
    layout->addWidget(this->_file_adder);
    layout->addWidget(this->_file_list);
    layout->addWidget(this->_footer);
  }

  connect(this->_file_adder->attach(),
          SIGNAL(released()),
          this,
          SIGNAL(choose_files()));

  connect(this->footer()->send(), SIGNAL(clicked()), this, SLOT(send()));

  this->connect(this->_search, SIGNAL(textChanged(QString const&)),
                SLOT(_search_changed(QString const&)));
}

void
SendPanel::_search_changed(QString const& search)
{
  QStringList res;

  if (search.size() != 0)
  {
    std::string text(search.toStdString());
    uint32_t* uids = gap_search_users(_state, text.c_str());

    this->setUsers(uids);
    gap_search_users_free(uids);
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
  if (this->_files.insert(path, new FileItem(path)) != this->_files.end())
  {
    connect(this->_files[path], SIGNAL(remove(QString const&)),
            this, SLOT(remove_file(QString const&)));
    this->_file_list->addWidget(this->_files[path]);
  }

  if (this->_files.size() > 0)
    this->footer()->send()->enable();
}

void
SendPanel::remove_file(QString const& path)
{
  auto it = this->_files.find(path);

  if (it != this->_files.end())
  {
    this->_file_list->removeWidget(it.value());
    this->_files.remove(path);
  }

  if (this->_files.size() == 0)
    this->footer()->send()->disable();
}

/*------.
| Users |
`------*/

void
SendPanel::setUsers(uint32_t* uids)
{
  this->_users->clearWidgets();

  if (uids == nullptr)
    return;

  auto* uidscopy = uids;
  while (*uidscopy != gap_null())
  {
    auto uid = *uidscopy;
    if (this->_user_models.find(uid) == this->_user_models.end())
      this->_user_models.emplace(uid, UserModel(this->_state, uid));
    auto widget = new UserWidget(this->_user_models.at(uid), this);
    connect(widget,
            SIGNAL(clicked_signal(uint32_t)),
            this,
            SLOT(send(uint32_t)));
    this->_users->addWidget(widget);

    ++uidscopy;
  }
}

void
SendPanel::clearUsers()
{
  this->setUsers(nullptr);
}

void
SendPanel::send(uint32_t uid)
{
  if (uid == 0)
  {
    std::string text(this->_search->text().toStdString());
    uint32_t* uids = gap_search_users(_state, text.c_str());
    uid = uids[0];
    gap_search_users_free(uids);
  }

  if (this->_files.empty())
  {
    std::cerr << "NO FILES" << std::endl;
    return;
  }

  char** filenames;
  if ((filenames = (char**)malloc((this->_files.size() + 1) * sizeof(char*))) == nullptr)
  {
    std::cerr << "error: unable to allocate" << std::endl;
  }

  for (int i = 0; i < this->_files.size(); i++)
  {
    if ((filenames[i] = (char*)malloc((this->_files.keys().at(i).size() + 1))) == nullptr)
    {
      std::cerr << "error: unable to allocate" << std::endl;
    }

    strcpy(filenames[i], this->_files.keys().at(i).toStdString().c_str());
  }
  filenames[this->_files.size()] = nullptr;

  gap_send_files(_state, uid, filenames, "Basic comment");

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
SendPanel::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    emit switch_signal();
}

/*-------.
| Layout |
`-------*/
QSize
SendPanel::sizeHint() const
{
  return QSize(Super::sizeHint().width(),
               std::min(400, Super::sizeHint().height()));
}

/*------------.
| Show & Hide |
`------------*/
void
SendPanel::on_show()
{
  this->_search->setFocus();
}

void
SendPanel::on_hide()
{
  this->_search->clear();
  this->_file_list->clearWidgets();
  this->_files.clear();
}

/*-------.
| Footer |
`-------*/
SendFooter*
SendPanel::footer()
{
  return static_cast<SendFooter*>(this->_footer);
}
