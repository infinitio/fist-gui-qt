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

SendPanel::SendPanel():
  Super(),
  _search(nullptr),
  _users(nullptr)
{
  this->_users = new ListWidget(this, &this->_search);
  auto search = new SearchField(this, this->_users);
  this->_search = search;
  search->setIcon(QPixmap(":/icons/magnifier.png"));
  new SendFooter(this);

  this->connect(this->_search, SIGNAL(textChanged(QString const&)),
                SIGNAL(onSearchChanged(QString const&)));
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/*------.
| Files |
`------*/

void
SendPanel::addFile(QString const&)
{

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
SendPanel::setUsers(QStringList const& users)
{
  this->_users->clearWidgets();
  for (auto const& user: users)
  {
    auto widget = new UserWidget(user, this);
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

/*-------.
| Layout |
`-------*/

QSize
SendPanel::sizeHint() const
{
  return QSize(320, std::min(400, Super::sizeHint().height()));
}
