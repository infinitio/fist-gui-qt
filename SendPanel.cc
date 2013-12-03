#include <QEvent>
#include <QLineEdit>
#include <QPainter>
#include <QTextEdit>
#include <QGridLayout>

#include <fist-gui-qt/AvatarIcon.hh>
#include <fist-gui-qt/FileItem.hh>
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
    }
  };
}

SendPanel::SendPanel(gap_State* state):
  Super(new SendFooter),
  _users(new ListWidget(this)),
  _search(new SearchField(this, &this->_users)),
  _send(new QPushButton("Send", this)),
  _state(state),
  _file_list(new ListWidget(this))
{
  auto layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  this->_users->set_mate(this->_search);
  this->_users->setMaxRows(5);
  this->_file_list->setMaxRows(4);

  this->_search->setIcon(QPixmap(":/icons/magnifier.png"));

  layout->addWidget(this->_search, 0, 0);
  layout->addWidget(this->_send, 0, 1);
  layout->addWidget(this->_users, 1, 0, 1, -1);
  layout->addWidget(new Separator, 2, 0, 1, -1);
  // layout->addWidget(new Note, 2, 0, 2, 3)
  // layout->addWidget(new Separator, 3, 0, 3, 3);
  layout->addWidget(this->_file_list, 3, 0, 1, -1);
  layout->addWidget(this->_footer, 4, 0, 1, -1);

  connect(this->_send, SIGNAL(clicked()), this, SLOT(send()));

  this->connect(this->_search, SIGNAL(textChanged(QString const&)),
                SLOT(_search_changed(QString const&)));
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
SendPanel::addFile(QString const& path)
{
  auto res = this->_files.insert(path);
  if (res.second)
    this->_file_list->addWidget(new FileItem(path));
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
    emit switch_signal();
    return;
  }

  const char* filenames[2] = { 0 };
  filenames[0] = this->_files.begin()->toStdString().c_str();

  gap_send_files(_state, uid, filenames, "Basic comment");

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
  return QSize(320, std::min(400, Super::sizeHint().height()));
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
