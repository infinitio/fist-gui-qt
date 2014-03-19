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

#include <elle/log.hh>

#include <fist-gui-qt/InfinitDock.hh>
#include <fist-gui-qt/RoundShadowWidget.hh>
#include <fist-gui-qt/SendPanel.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/UserPanel.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Dock");

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
    ELLE_TRACE_SCOPE("ininitialize Prologue");

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
  _user_panel(nullptr),
  _transaction_panel(nullptr),
  _send_panel(nullptr),
  _menu(new QMenu(this)),
  _logo(":/icons/menu-bar-fire@2x.png"),
  _systray(new QSystemTrayIcon(this)),
  _systray_menu(new QMenu(this)),
  _send_files(new QAction(tr("&Send files..."), this)),
  _quit(new QAction(tr("&Quit"), this)),
  _state(state),
  _first_hide(true)
{
  ELLE_TRACE_SCOPE("%s: creation", *this);
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

  connect(this, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));

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

  ELLE_TRACE("show user view")
    this->_show_user_view();

  QTimer *timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(_update()));
  timer->start(1000);

  this->connect(_send_files, SIGNAL(triggered()), this, SLOT(pick_files()));
  this->connect(_quit, SIGNAL(triggered()), this, SIGNAL(quit_request()));
  this->show();
  this->show_dock();
}

InfinitDock::~InfinitDock()
{
  ELLE_TRACE_SCOPE("%s: destruction", *this);
}

void
InfinitDock::_systray_activated(QSystemTrayIcon::ActivationReason reason)
{
  ELLE_TRACE_SCOPE("%s: system activated: %s", *this, reason);

  switch (reason)
  {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Unknown:
    case QSystemTrayIcon::MiddleClick:
      this->toggle_dock();
      break;
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Context:
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
  ELLE_TRACE_SCOPE("%s: show system tray message: %s - %s",
                   *this, title, message);
  this->_systray->showMessage(title, message, icon, 3000);
}

/*------.
| Panel |
`------*/

void
InfinitDock::_register_panel(Panel* panel)
{
  ELLE_ASSERT(panel != nullptr);

  ELLE_TRACE_SCOPE("%s: register panel %s", *this, *panel);

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
InfinitDock::transaction_panel()
{
  if (this->_transaction_panel == nullptr)
  {
    this->_transaction_panel.reset(new TransactionPanel(this->_state));
    this->_register_panel(this->_transaction_panel.get());

    connect(this,
            SIGNAL(avatar_available(uint32_t)),
            this->_transaction_panel.get(),
            SLOT(avatar_available(uint32_t)));

    connect(this,
            SIGNAL(user_status_changed(uint32_t, gap_UserStatus)),
            this->_transaction_panel.get(),
            SLOT(user_status_changed(uint32_t, gap_UserStatus)));

    connect(&this->_transaction_panel->footer()->send(),
            SIGNAL(released()),
            this,
            SLOT(_show_send_view()));
    connect(&this->_transaction_panel->footer()->back(),
            SIGNAL(released()),
            this,
            SLOT(_show_user_view()));
  }

  return *this->_transaction_panel;
}

UserPanel&
InfinitDock::user_panel()
{
  if (this->_user_panel == nullptr)
  {
    this->_user_panel.reset(new UserPanel(this->_state));
    this->_register_panel(this->_user_panel.get());

    {
      connect(this->_user_panel.get(), SIGNAL(user_clicked(uint32_t)),
              this, SLOT(_show_transactions_view(uint32_t)));

    connect(&this->_user_panel->footer()->send(),
            SIGNAL(released()),
            this,
            SLOT(_show_send_view()));
    connect(&this->_user_panel->footer()->menu(),
            SIGNAL(released()),
            this,
            SLOT(_show_menu()));

    }
  }

  return *this->_user_panel;
}

SendPanel&
InfinitDock::send_panel()
{
  if (this->_send_panel == nullptr)
  {
    this->_send_panel.reset(new SendPanel(this->_state));
    this->_register_panel(this->_send_panel.get());

    {
      connect(this->_send_panel->footer()->back(),
              SIGNAL(released()),
              this,
              SLOT(_back_from_send_view()));

      connect(this->_send_panel.get(),
              SIGNAL(switch_signal()),
              this,
              SLOT(_back_from_send_view()));

      connect(this->_send_panel.get(),
              SIGNAL(choose_files()),
              this,
              SLOT(pick_files()));

      connect(this,
              SIGNAL(avatar_available(uint32_t)),
              this->_send_panel.get(),
              SLOT(avatar_available(uint32_t)));
    }
  }

  return *this->_send_panel;
}

void
InfinitDock::show_dock()
{
  ELLE_TRACE_SCOPE("%s: show dock", *this);

  this->show();
  this->update();
  this->activateWindow();
  this->setFocus(Qt::ActiveWindowFocusReason);
  this->_position_panel();
}

void
InfinitDock::hide_dock()
{
  ELLE_TRACE_SCOPE("%s: hide dock", *this);

  this->hide();

  if (this->_first_hide)
  {
    this->_first_hide = false;
    this->_systray->showMessage(QString("Infinit is minimized!"),
                                QString("Make sure the Infinit icon is always "
                                        "visible by clicking customize!"),
                                QSystemTrayIcon::Information,
                                60000);
  }
}

void
InfinitDock::toggle_dock(bool toggle_only)
{
  ELLE_TRACE_SCOPE("%s: toggle dock", *this);

  if (this->isVisible() and !toggle_only)
    this->hide_dock();
  else
    this->show_dock();
}

void
InfinitDock::_position_panel()
{
  ELLE_TRACE_SCOPE("%s: move panel", *this);

  ELLE_DUMP("%s: old position: (%s, %s)", *this, this->x(), this->y());

  QPoint systray_position(this->_systray->geometry().center());

  auto screen = QDesktopWidget().availableGeometry();

  auto x = systray_position.x() - this->width() / 2;
  auto y = systray_position.y() - this->height() / 2;

  static int margin = 5;

  this->move(
    qBound(screen.left() + margin,
           x,
           screen.right() - margin - this->width()),
    qBound(screen.top() + margin,
           y,
           screen.bottom() - margin - this->height()));

  ELLE_DUMP("%s: new position: (%s, %s)", *this, this->x(), this->y());
}


void
InfinitDock::pick_files()
{
  ELLE_TRACE_SCOPE("%s: spawn file picker", *this);

  QStringList selected = QFileDialog::getOpenFileNames(
    this,
    tr("Select files to send"));

  if (selected.size())
  {
    for (auto const& file: selected)
      this->_send_panel->add_file(QUrl::fromLocalFile(file));
    this->_show_send_view();
    this->show_dock();
  }
}

void
InfinitDock::enterEvent(QEvent* event)
{
  if (this->centralWidget() != nullptr)
    ELLE_DEBUG("%s currently active", *this->centralWidget());

  if (this->centralWidget() == this->_send_panel.get())
    this->setFocus();
}

void
InfinitDock::closeEvent(QCloseEvent* event)
{
  ELLE_TRACE_SCOPE("%s: close", *this);

  Super::closeEvent(event);
  this->deleteLater();
}

void
InfinitDock::mouseReleaseEvent(QMouseEvent* event)
{
  ELLE_TRACE_SCOPE("%s: mouse released", *this);

  if (event->button() == Qt::LeftButton)
  {
    ELLE_DEBUG("left button clicked");

    event->accept();
    this->toggle_dock(true);
  }
  else
    Super::mouseReleaseEvent(event);
}

void
InfinitDock::focusInEvent(QFocusEvent* event)
{
  ELLE_TRACE_SCOPE("%s: get focus (%s)", *this, event->reason());

  this->update();
  this->_position_panel();

  if (this->centralWidget() == nullptr)
  {
    this->centralWidget()->setFocus(event->reason());
  }
}

void
InfinitDock::keyPressEvent(QKeyEvent* event)
{
  ELLE_TRACE_SCOPE("%s: key pressed (%s)", *this, event->key());

  if (this->centralWidget() != nullptr)
    ELLE_DEBUG("%s currently active", *this->centralWidget());
  else if (this->centralWidget() == this->_user_panel.get())
  {
    if (event->key() == Qt::Key_Escape)
    {
      ELLE_DEBUG("escape pressed");
      this->toggle_dock();
    }
    else if (event->key() == Qt::Key_S)
    {
      this->_show_send_view();
    }
  }
  else if (this->centralWidget() == this->_send_panel.get())
  {
    if (event->key() == Qt::Key_Escape)
    {
      ELLE_DEBUG("escape pressed");
      this->toggle_dock();
    }
  }
  else if (this->centralWidget() == this->_transaction_panel.get())
  {
    if (event->key() == Qt::Key_R)
    {
      this->transaction_panel().update();
    }
  }
}

void
InfinitDock::_show_send_view()
{
  ELLE_TRACE_SCOPE("%s: show send view", *this);
  this->_switch_view(&this->send_panel());
}

void
InfinitDock::_show_user_view()
{
  ELLE_TRACE_SCOPE("%s: show user view", *this);
  this->_switch_view(&this->user_panel());
}

void
InfinitDock::_show_menu()
{
  ELLE_TRACE_SCOPE("%s: show menu", *this);

  this->_menu->show();

  QPoint pos(this->geometry().bottomLeft());
  this->_menu->move(pos);
}

void
InfinitDock::_back_from_send_view()
{
  this->_show_user_view();
  this->hide_dock();
}

void
InfinitDock::focusOutEvent(QFocusEvent* event)
{
  ELLE_TRACE_SCOPE("%s: focus lost (reason %s)", *this, event->reason());

  if (this->centralWidget() != nullptr &&
      this->centralWidget() == this->_send_panel.get())
  {
    event->accept();
    return;
  }

  Super::focusOutEvent(event);

  if (event->reason() != Qt::MouseFocusReason)
    this->hide_dock();
}


void
InfinitDock::_show_transactions_view(uint32_t sender_id)
{
  ELLE_TRACE_SCOPE("%s: show transaction view", *this);

  this->transaction_panel().peer(&this->user_panel().users().at(sender_id));
  this->_switch_view(&this->transaction_panel());
}

void
InfinitDock::_switch_view(Panel* panel)
{
  ELLE_ASSERT(panel != nullptr);

  ELLE_TRACE_SCOPE("%s: make %s active", *this, *panel);

  if (panel == this->centralWidget())
  {
    ELLE_DEBUG("%s was already active", *panel);
    return;
  }

  if (this->centralWidget() != nullptr)
  {
    ELLE_DEBUG("hide %s", *this->centralWidget());
    static_cast<Panel*>(this->centralWidget())->on_hide();
    this->centralWidget()->setParent(0);
  }

  this->setCentralWidget(panel);
  panel->on_show();
  this->update();
}

void
InfinitDock::connection_status_cb(gap_UserStatus const status)
{
  ELLE_TRACE_SCOPE("Dock: Connection status changed to %s", status);
}

void
InfinitDock::user_status_cb(uint32_t id,
                            gap_UserStatus const status)
{
  ELLE_TRACE_SCOPE("Dock: User (%s) status changed to %s", id, status);

  g_dock->user_status_changed(id, status);
}

void
InfinitDock::avatar_available_cb(uint32_t id)
{
  ELLE_TRACE_SCOPE("Dock: Avatar available for user %s", id);

  g_dock->avatar_available(id);
}

void
InfinitDock::_update()
{
  ELLE_DUMP("%s: poll", *this);
  auto res = gap_poll(_state);

  if (!res)
    ELLE_ERR("poll failed: %s", res);
}

void
InfinitDock::print(std::ostream& stream) const
{
  stream << "Dock";
}
