#include <iostream>

#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QDesktopWidget>
#include <QSystemTrayIcon>
#include <QUrl>
#include <QWidgetAction>

#include <version.hh>

#include <fist-gui-qt/InfinitDock.hh>
#include <fist-gui-qt/RoundShadowWidget.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/utils.hh>

/*-------------.
| Construction |
`-------------*/

static int const dock_size = 60;

// XXX: This is dirty but there is no good way to emit a signal from a static
// method. Unfortunately, gap api, which is in C, forces to attach C callbacks.
// Attaching a static method works but make signal emission impossible.
// The only 'good' way I found, while there is only one instance of InfinitDock
// is to add a global pointer to the dock and makes it emit the signal.
InfinitDock* g_dock = nullptr;

class InfinitDock::Prologue
{
  friend InfinitDock;

  Prologue(gap_State* state)
  {
    // Register gap callback.
    gap_connection_callback(state, InfinitDock::connection_status_cb);
    gap_user_status_callback(state, InfinitDock::user_status_cb);
    gap_avatar_available_callback(state, InfinitDock::avatar_available_cb);
  }
};

// Creating the transaction panel is a long operation. So we just wait until all
// the graphical part is fully load, and then, initialize it.
InfinitDock::InfinitDock(gap_State* state):
  _prologue(new Prologue(state)),
  _transaction_panel(nullptr),
//  _panel(new RoundShadowWidget),
  _send_panel(new SendPanel(state)),
  _menu(new QMenu(this)),
  _logo(":/icons/menu-bar-fire@2x.png"),
  _systray(new QSystemTrayIcon(this)),
  _systray_menu(new QMenu(this)),
  _send_files(new QAction(tr("&Send files..."), this)),
  _file_picker(nullptr),
  _quit(new QAction(tr("&Quit"), this)),
  _state(state)
{
  g_dock = this;

  QIcon icon(this->_logo);
  _systray->setIcon(icon);
  _systray->setVisible(true);
  connect(_systray,
          SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,
          SLOT(_systray_activated(QSystemTrayIcon::ActivationReason)));

  this->_systray_menu->addAction(_send_files);
  this->_systray_menu->addAction(_quit);
  this->_systray->setContextMenu(_systray_menu);

  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_TranslucentBackground, true);

  this->_transaction_panel = new TransactionPanel(state);

  this->_register_panel(this->_transaction_panel);
  this->_register_panel(this->_send_panel);

  connect(this, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));

  {
    connect(&this->_transaction_panel->footer()->send(),
            SIGNAL(released()),
            this,
            SLOT(_show_send_view()));
    connect(&this->_transaction_panel->footer()->menu(),
            SIGNAL(released()),
            this,
            SLOT(_show_menu()));
  }

  {
    connect(this->_send_panel->footer()->back(),
            SIGNAL(released()),
            this,
            SLOT(_show_transactions_view()));

    connect(this->_send_panel,
            SIGNAL(choose_files()),
            this,
            SLOT(pick_files()));

    connect(this->_send_panel,
            SIGNAL(switch_signal()),
            this,
            SLOT(_show_transactions_view()));
  }

  connect(this,
          SIGNAL(avatar_available(uint32_t)),
          this->_send_panel,
          SLOT(avatar_available(uint32_t)));

  connect(this,
          SIGNAL(avatar_available(uint32_t)),
          this->_transaction_panel,
          SLOT(avatar_available(uint32_t)));

  connect(this,
          SIGNAL(user_status_changed(uint32_t, gap_UserStatus)),
          this->_transaction_panel,
          SLOT(user_status_changed(uint32_t, gap_UserStatus)));

  // XXX: Specialize a QWidgetAction to add a better visual and for example,
  // to copy the version in the user clipboard on click.
  QWidgetAction* version = new QWidgetAction(this->_menu);
  version->setDefaultWidget(new QLabel(QString(INFINIT_VERSION)));
  this->_menu->addAction(version);
  this->_menu->addSeparator();
  this->_menu->addAction(_quit);

  // Register gap callback.
  gap_connection_callback(_state, InfinitDock::connection_status_cb);
  gap_user_status_callback(_state, InfinitDock::user_status_cb);
  gap_avatar_available_callback(_state, InfinitDock::avatar_available_cb);

  this->_switch_view(this->_transaction_panel);

  QTimer *timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(500);

  this->connect(_send_files, SIGNAL(triggered()), this, SLOT(pick_files()));
  this->connect(_quit, SIGNAL(triggered()), this, SLOT(quit()));
  this->show();
  this->showPanel();
}

void
InfinitDock::_systray_activated(QSystemTrayIcon::ActivationReason reason)
{
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
    default:
      break;
  }
}

void
InfinitDock::_systray_message(QString const& title,
                              QString const& message,
                              QSystemTrayIcon::MessageIcon icon)
{
  this->_systray->showMessage(title, message, icon, 3000);
}

/*------.
| Panel |
`------*/

void
InfinitDock::_register_panel(Panel* panel)
{
  connect(panel, SIGNAL(systray_message(QString const&,
                                        QString const&,
                                        QSystemTrayIcon::MessageIcon)),
          this, SLOT(_systray_message(QString const&,
                                      QString const&,
                                      QSystemTrayIcon::MessageIcon)));

  connect(panel, SIGNAL(set_background_color(QColor const&)),
          this, SLOT(setBackground(QColor const&)));
}

TransactionPanel&
InfinitDock::transactionPanel()
{
  return *this->_transaction_panel;
}

void
InfinitDock::showPanel()
{
  this->show();
  this->setFocus();
  this->_position_panel();
}

void
InfinitDock::hidePanel()
{
  this->hide();
}

void
InfinitDock::togglePanel(bool toggle_only)
{
  if (this->isVisible() and !toggle_only)
    this->hidePanel();
  else
    this->showPanel();
}

void
InfinitDock::_position_panel()
{
  QPoint systray_position(this->_systray->geometry().topLeft());

  auto screen = QDesktopWidget().availableGeometry();

  auto x = systray_position.x() - this->width();
  auto y = systray_position.y() - this->height() - 32;

  this->move(qBound(screen.left(), x, screen.right()),
             qBound(screen.top(), y, screen.bottom()));
}

static
QFileDialog*
spaw_dialog(InfinitDock* dock)
{
  std::unique_ptr<QFileDialog> file_picker(new QFileDialog(dock));
  file_picker->setFileMode(QFileDialog::ExistingFiles);
  return file_picker.release();

}

void
InfinitDock::pick_files()
{
  this->_file_picker.reset(spaw_dialog(this));
  this->_file_picker->exec();
  QStringList selected = this->_file_picker->selectedFiles();

  if (selected.size())
  {
    for (auto const& file: selected)
      this->_send_panel->add_file(file);
    this->_switch_view(this->_send_panel);
    this->showPanel();
  }
  this->_file_picker.reset();
}

void
InfinitDock::quit()
{
    this->hide();
    this->_send_panel->hide();
    this->_transaction_panel->hide();
    this->_systray->hide();
    this->_systray_menu->hide();
    this->deleteLater();

    QApplication::setQuitOnLastWindowClosed(true);
    QApplication::quit();
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
        this->_send_panel->add_file(url.toLocalFile());
      }
  this->_switch_view(this->_send_panel);
  this->showPanel();
}

void
InfinitDock::closeEvent(QCloseEvent* event)
{
  this->close();
  Super::closeEvent(event);
  qApp->quit();
}

void
InfinitDock::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    event->accept();
    this->togglePanel(true);
  }
  else
    Super::mouseReleaseEvent(event);
}

void
InfinitDock::keyPressEvent(QKeyEvent* event)
{
  if (this->centralWidget() == this->_transaction_panel)
  {
    if (event->key() == Qt::Key_Escape)
      this->togglePanel();
  }
  else if (this->centralWidget() == this->_send_panel)
  {
    if (event->key() == Qt::Key_Escape)
      this->togglePanel();
  }
}

void
InfinitDock::_show_send_view()
{
  this->_switch_view(this->_send_panel);
}

void
InfinitDock::_show_user_view(uint32_t /* sender_id */)
{

}

void
InfinitDock::_show_menu()
{
  this->_menu->show();

  QPoint pos(this->geometry().bottomLeft());
  this->_menu->move(pos);
}

void
InfinitDock::_show_transactions_view()
{
  this->_switch_view(this->_transaction_panel);
}

void
InfinitDock::_switch_view(Panel* panel)
{
  if (panel == this->centralWidget())
  {
    return;
  }

  if (this->centralWidget() != nullptr)
  {
    static_cast<Panel*>(this->centralWidget())->on_hide();
    this->centralWidget()->setParent(0);
  }

  this->setCentralWidget(panel);
  panel->on_show();
}

void
InfinitDock::connection_status_cb(gap_UserStatus const status)
{
 // if (status == gap_user_status_offline)
  std::cout << "Connection status callback: " << status << std::endl;
}

void
InfinitDock::user_status_cb(uint32_t id,
                            gap_UserStatus const status)
{
  std::cerr << "User status changed" << std::endl;
  g_dock->user_status_changed(id, status);
}

void
InfinitDock::avatar_available_cb(uint32_t id)
{
  std::cerr << "Avatar available (" << id << ")" << std::endl;
  g_dock->avatar_available(id);
}

void
InfinitDock::update()
{
  if (not gap_poll(_state))
    std::cout << "Error while polling" << std::endl;
}
