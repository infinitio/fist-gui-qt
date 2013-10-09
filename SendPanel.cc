#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>

#include "Footer.hh"
#include "IconButton.hh"
#include "ListWidget.hh"
#include "SearchField.hh"
#include "SendPanel.hh"
#include "utils.hh"

class SendFooter:
  public Footer
{
public:
  SendFooter(QWidget* parent = nullptr):
    Footer(parent)
  {
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 5);
    layout->addWidget(new IconButton(QPixmap(":/icons/gear.png"), true));
    layout->addItem(new QSpacerItem(0, 0,
                                    QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Minimum));
    layout->addWidget(new IconButton(QPixmap(":/icons/arrows.png"), true));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }
};

/*-------------.
| Construction |
`-------------*/

SendPanel::SendPanel(gap_State* state):
  Super(),
  _search(nullptr),
  _send(new QPushButton("Send", this)),
  _users(nullptr),
  _state(state)
{
  this->_users = new ListWidget(this);
  auto search = new SearchField(this, &this->_users);
  this->_users->set_mate(search);
  this->_search = search;
  search->setIcon(QPixmap(":/icons/magnifier.png"));
  new SendFooter(this);

  connect(this->_send, SIGNAL(clicked()), this, SLOT(send()));

  this->connect(this->_search, SIGNAL(textChanged(QString const&)),
                SIGNAL(onSearchChanged(QString const&)));
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/*------.
| Files |
`------*/

void
SendPanel::addFile(QString const& path)
{
  _file_path = std::string(path.toStdString());
}

/*------.
| Users |
`------*/

class AvatarIcon:
  public QWidget
{
public:
  AvatarIcon(QPixmap const& pixmap):
    _pixmap(QSize(30, 30))
  {
    QSize size(30, 30);
    this->setMinimumSize(size);
    this->setMaximumSize(size);
    QRect geometry(QPoint(0, 0), size);
    this->_pixmap.fill(Qt::transparent);
    QPixmap mask(size);
    {
      mask.fill(Qt::transparent);
      QPainter painter(&mask);
      painter.setRenderHints(QPainter::Antialiasing |
                             QPainter::SmoothPixmapTransform);
      painter.setPen(Qt::NoPen);
      painter.setBrush(Qt::black);
      painter.drawEllipse(geometry);
    }
    QPainter painter(&this->_pixmap);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.drawPixmap(geometry, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawPixmap(geometry, mask);
  }

protected:
  virtual
  void
  paintEvent(QPaintEvent*) override
  {
    QRect geometry(QPoint(0, 0), this->size());
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.drawPixmap(geometry, this->_pixmap);
  }

private:
  QPixmap _pixmap;
};

void
SendPanel::setUsers(QStringList const& users, uint32_t* uids)
{
  this->_users->clearWidgets();

  if (uids == nullptr)
    return;

  uint32_t i = 0;
  for (auto const& user: users)
  {
    auto widget = new UserWidget(user, uids[i++], this);
    connect(widget, SIGNAL(clicked_signal(uint32_t)), this, SLOT(send(uint32_t)));

    auto layout = new QHBoxLayout(widget);
    layout->addWidget(new AvatarIcon(QPixmap("resources/avatar3.png")));
    layout->addWidget(new QLabel(user));
    this->_users->addWidget(widget);
  }
}

void
SendPanel::clearUsers()
{
  this->setUsers(QStringList());
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

  const char* filenames[2] = { 0 };
  filenames[0] = _file_path.c_str();

  gap_send_files(_state, uid, filenames, "Basic comment");

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
