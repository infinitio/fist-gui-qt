#include <QApplication>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QUrl>
#include <QMenu>
#include <QAction>

#include "InfinitDock.hh"
#include "RoundShadowWidget.hh"
#include "SendPanel.hh"
#include "TransactionPanel.hh"
#include "utils.hh"

/*-------------.
| Construction |
`-------------*/

static int const dock_size = 60;
static InfinitDock* g_dock = nullptr;

InfinitDock::InfinitDock(gap_State* state):
  _transaction_panel(new TransactionPanel(state)),
  _panel(new RoundShadowWidget),
  _send_panel(new SendPanel(state)),
  _logo(":/images/logo.png"),
  _systray(new QSystemTrayIcon(this)),
  _systray_menu(new QMenu(this)),
  _send_files(new QAction(tr("&Send files..."), this)),
  _choose_files(new QFileDialog(this)),
  _quit(new QAction(tr("&Quit"), this)),
  _state(state)
{
  this->_choose_files->setFileMode(QFileDialog::ExistingFiles);

  this->_systray_menu->addAction(_send_files);
  this->_systray_menu->addAction(_quit);
  this->_systray->setContextMenu(_systray_menu);


  // Register gap callback.
  g_dock = this;
  gap_connection_callback(_state, InfinitDock::connection_status_cb);
  gap_user_status_callback(_state, InfinitDock::user_status_cb);

  this->_panel->setCentralWidget(this->_transaction_panel);
  this->resize(dock_size, dock_size);
  //
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  QRect const rect(QApplication::desktop()->rect());
  this->move(rect.width() - 100, rect.height() - 100);
  connect(this->_panel, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));
  connect(this->_send_panel, SIGNAL(onSearchChanged(QString const&)),
          SLOT(_search(QString const&)));
  this->setAcceptDrops(true);
  this->_transaction_panel->setFocus();

  QTimer *timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(2000);

  connect(_send_panel, SIGNAL(switch_signal()), this, SLOT(switch_panel()));
  QIcon icon(this->_logo);
  this->setWindowIcon(icon);
  _systray->setIcon(icon);

  _systray->setVisible(true);

  connect(_systray,
          SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,
          SLOT(_systray_activated(QSystemTrayIcon::ActivationReason)));
  this->connect(_send_files, SIGNAL(triggered()), this, SLOT(chooseFiles()));
  this->connect(_quit, SIGNAL(triggered()), this, SLOT(quit()));
}

void
InfinitDock::_systray_activated(QSystemTrayIcon::ActivationReason reason)
{
    printf("Systray\n");
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Unknown:
    case QSystemTrayIcon::MiddleClick:
      this->togglePanel();
      break;
    case QSystemTrayIcon::Context:
      this->hidePanel();
      break;
    }
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

  QStringList res;

  if (search.size() != 0)
  {
    std::string text(search.toStdString());
    uint32_t* uids = gap_search_users(_state, text.c_str());

    for (uint32_t i = 0; uids[i] != 0; i += 1)
      res.append(QString(gap_user_fullname(_state, uids[i])));

    this->_send_panel->setUsers(res, uids);
    gap_search_users_free(uids);
  }
  else
    this->_send_panel->clearUsers();

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
  this->_panel->centralWidget()->setParent(0);
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
InfinitDock::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Return)
    this->togglePanel();
  else if (event->key() == Qt::Key_Escape)
    this->deleteLater();
}

void
InfinitDock::switch_panel()
{
  if (this->_panel->centralWidget() == this->_send_panel)
  {
    this->_panel->centralWidget()->setParent(0);
    this->_panel->setCentralWidget(this->_transaction_panel);
  }
  else
  {
    this->_panel->centralWidget()->setParent(0);
    this->_panel->setCentralWidget(this->_send_panel);
  }

  this->showPanel();
}

void
InfinitDock::connection_status_cb(gap_UserStatus const status)
{
 // if (status == gap_user_status_offline)
  std::cout << "Connection status callback: " << status << std::endl;
}

void
InfinitDock::user_status_cb(uint32_t id, gap_UserStatus const status)
{
  std::cerr << "User status changed" << std::endl;
  //g_dock->_send_panel->update_list(id, status);
}

void
InfinitDock::update()
{
  if (not gap_poll(_state))
    std::cout << "Error while polling" << std::endl;
}
