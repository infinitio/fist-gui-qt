#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QUrl>

#include "InfinitDock.hh"
#include "RoundShadowWidget.hh"
#include "SendPanel.hh"
#include "TransactionPanel.hh"
#include "utils.hh"

/*-------------.
| Construction |
`-------------*/

static int const dock_size = 60;

InfinitDock::InfinitDock():
  _transaction_panel(new TransactionPanel),
  _panel(new RoundShadowWidget),
  _send_panel(new SendPanel),
  _logo(":/images/logo.png"),
  _background(dock_size, dock_size)
{
  // Cache background
  {
  this->_background.fill(Qt::transparent);
  QPainter painter(&this->_background);
  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setBrush(Qt::black);
  painter.setPen(Qt::NoPen);
  int const logo_size = dock_size / 2;//sqrt(2);
  QPixmap logo_white(logo_size, logo_size);
  {
    logo_white.fill(Qt::transparent);
    logo_white.fill(Qt::white);
    QPainter painter(&logo_white);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawPixmap(QRect(0, 0, logo_size, logo_size), this->_logo);
  }
  painter.setOpacity(0.5);
  painter.drawEllipse(QRect(QPoint(0, 0), QSize(dock_size, dock_size)));
  painter.setOpacity(1.);
  painter.drawPixmap(QPoint((dock_size - logo_size) / 2,
                            (dock_size - logo_size) / 2), logo_white);
  }
  this->_panel->setCentralWidget(this->_transaction_panel);
  this->resize(dock_size, dock_size);
  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  QRect const rect(QApplication::desktop()->rect());
  this->move(rect.width() - 100, rect.height() - 100);
  connect(this->_panel, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));
  connect(this->_send_panel, SIGNAL(onSearchChanged(QString const&)),
          SLOT(_search(QString const&)));
  this->setAcceptDrops(true);
  this->_transaction_panel->setFocus();
}

/*------.
| Panel |
`------*/

TransactionPanel&
InfinitDock::transactionPanel()
{
  return *this->_transaction_panel;
}

void
InfinitDock::showPanel()
{
  this->_panel->show();
  this->_position_panel();
}

void
InfinitDock::hidePanel()
{
  this->_panel->hide();
}

void
InfinitDock::togglePanel()
{
  if (this->_panel->isVisible())
    this->hidePanel();
  else
    this->showPanel();
}

void
InfinitDock::_position_panel()
{
  QPoint pos(this->pos());
  auto x = pos.x() + this->width() - this->_panel->width();
  auto y = pos.y() - this->_panel->height();
  this->_panel->move(x, y);
}

/*-----.
| Send |
`-----*/

void
InfinitDock::_search(QString const& search)
{
  QStringList users;
  users.append("Chris");
  users.append("Chris");
  users.append("Dimrok");
  users.append("Dimrok");
  users.append("MyCure");
  users.append("MyCure");
  users.append("Raph");
  users.append("Raph");
  users.append("mefyl");
  users.append("mefyl");

  if (search == "")
  {
    this->_send_panel->clearUsers();
    return;
  }
  QRegExp re(search, Qt::CaseInsensitive);
  QStringList res;
  for (auto const& user: users)
  {
    if (re.indexIn(user) != -1)
      res.append(user);
  }
  this->_send_panel->setUsers(res);
}

/*--------------.
| Drag and drop |
`--------------*/

void
InfinitDock::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        return;
      }
}

void
InfinitDock::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasUrls())
    for (auto const& url: event->mimeData()->urls())
      if (url.isLocalFile())
      {
        event->acceptProposedAction();
        this->_send_panel->addFile(url.toLocalFile());
      }
  this->_panel->setCentralWidget(this->_send_panel);
  this->showPanel();
}

void
InfinitDock::closeEvent(QCloseEvent* event)
{
  this->_panel->close();
  Super::closeEvent(event);
  qApp->quit();
}

void
InfinitDock::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    event->accept();
    this->togglePanel();
  }
  else
    Super::mouseReleaseEvent(event);
}

void
InfinitDock::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
  painter.drawPixmap(QPoint(0, 0), this->_background);
}
