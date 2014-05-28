#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QUrl>
#include <QWidgetAction>

#include <elle/log.hh>
#include <elle/os/environ.hh>

#include <version.hh>
#include <common/common.hh>
#include <surface/gap/gap.hh>

#include <fist-gui-qt/InfinitDock.hh>
#include <fist-gui-qt/RoundShadowWidget.hh>
#include <fist-gui-qt/SendView/Panel.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/onboarding/Onboarder.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Dock");

static const QString
onboarded_reception_complete("reception_complete");
static const QString
onboarded_sending_complete("sending_complete");

/*-------------.
| Construction |
`-------------*/

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
InfinitDock::InfinitDock(fist::State& state)
  : _prologue(new Prologue(state.state()))
  , _state(state)
  , _transaction_panel(nullptr)
  , _send_panel(new fist::sendview::Panel(this->_state))
  , _next_panel(nullptr)
  , _menu(new QMenu(this))
  , _logo(":/icons/menu-bar-fire@2x.png")
  , _systray(new QSystemTrayIcon(this))
  , _systray_menu(new QMenu(this))
  , _send_files(new QAction(tr("&Send files..."), this))
  , _report_a_problem(new QAction(tr("&Report a problem"), this))
  , _quit(new QAction(tr("&Quit"), this))
#ifndef FIST_PRODUCTION_BUILD
  , _start_onboarding_action(new QAction(tr("&Start onboarding"), this))
#endif
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

  this->connect(this->_systray, SIGNAL(messageClicked()),
                this, SLOT(_systray_message_clicked()));

  this->_transaction_panel = new MainPanel(this->_state,
                                           this);

  this->_register_panel(this->_transaction_panel);
  this->_register_panel(this->_send_panel);

  connect(this, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));

  {
    connect(this->_transaction_panel,
            SIGNAL(systray_message(QString const&, QString const&, QSystemTrayIcon::MessageIcon)),
            this, SLOT(_systray_message(QString const&, QString const&, QSystemTrayIcon::MessageIcon)));
    connect(this->_transaction_panel->footer()->send(),
            SIGNAL(released()),
            this,
            SLOT(_show_send_view()));
    connect(this->_transaction_panel->footer()->menu(),
            SIGNAL(released()),
            this,
            SLOT(_show_menu()));
  }

  {
    connect(this->_send_panel->footer()->back(),
            SIGNAL(released()),
            this,
            SLOT(_back_from_send_view()));

    connect(this->_send_panel,
            SIGNAL(switch_signal()),
            this,
            SLOT(_back_from_send_view()));

    connect(this->_send_panel,
            SIGNAL(choose_files()),
            this,
            SLOT(pick_files()));
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
  QWidgetAction* version = new QWidgetAction(this);
  version->setDefaultWidget(new QLabel(QString(INFINIT_VERSION)));
  this->_menu->addAction(version);
  this->_menu->addSeparator();
  this->_menu->addAction(_report_a_problem);
  this->_menu->addSeparator();
  this->_menu->addAction(_quit);

  // Register gap callback.
  gap_connection_callback(_state.state(), InfinitDock::connection_status_cb);
  gap_user_status_callback(_state.state(), InfinitDock::user_status_cb);
  gap_avatar_available_callback(_state.state(), InfinitDock::avatar_available_cb);

  this->_show_transactions_view();

  this->connect(_send_files, SIGNAL(triggered()), this, SLOT(pick_files()));
  this->connect(_report_a_problem, SIGNAL(triggered()),
                this, SLOT(report_a_problem()));
  this->connect(_quit, SIGNAL(triggered()), this, SIGNAL(quit_request()));

  ELLE_DEBUG("check if onboarded reception has been done")
    if (!fist::settings()["onboarding"].exists(onboarded_reception_complete))
    {
      int delay = 1000;
      ELLE_TRACE_SCOPE("run onboarded reception in %s ms", delay);
      auto* delay_onboarding = new QTimer(this);
      delay_onboarding->setSingleShot(true);
      connect(delay_onboarding, SIGNAL(timeout()),
              this, SLOT(_start_onboarded_reception()));
      delay_onboarding->start(delay);
    }

  this->hide();

#ifndef FIST_PRODUCTION_BUILD
  this->_menu->addAction(_start_onboarding_action);
  this->connect(this->_start_onboarding_action, SIGNAL(triggered()),
                this, SLOT(_start_onboarded_reception()));
#endif
}

InfinitDock::~InfinitDock()
{
  ELLE_TRACE_SCOPE("%s: quit", *this);
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
    case QSystemTrayIcon::DoubleClick:
      this->toggle_dock();
      break;
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
  if (!this->isVisible())
    this->_systray->showMessage(title, message, icon, 3000);
}

void
InfinitDock::_systray_message_clicked()
{
  this->show();
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
  panel->hide();
}

MainPanel&
InfinitDock::transactionPanel()
{
  return *this->_transaction_panel;
}

void
InfinitDock::showEvent(QShowEvent* event)
{
  ELLE_LOG_SCOPE("%s: show dock", *this);

  this->updateGeometry();
  this->update();
  this->activateWindow();
  this->setFocus(Qt::ActiveWindowFocusReason);
  this->_position_panel();
}

void
InfinitDock::hideEvent(QHideEvent* event)
{
  ELLE_LOG_SCOPE("%s: hide dock", *this);

  if (fist::settings()["onboarding"].exists(onboarded_reception_complete) &&
      !fist::settings()["dock"].exists("first_minimizing_popup"))
  {
    fist::settings()["dock"].set("first_minimizing_popup", "1");
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
    this->hide();
  else
    this->show();
}


void
InfinitDock::report_a_problem()
{
  bool ok;
  QString text = QInputDialog::getText(this,
                                       tr("Report a problem"),
                                       tr("Please describe the problem you had"),
                                       QLineEdit::Normal,
                                       "Enter your message",
                                       &ok);

  if (ok)
  {
    ELLE_DEBUG("user message: %s", text);

    auto log_file_picker = [] () -> std::string
    {
      for (std::string var: {"INFINIT_LOG_FILE", "ELLE_LOG_FILE"})
      {
        if (elle::os::inenv(var))
          return elle::os::getenv(var);
      }
      return "";
    };

    auto logfile = log_file_picker();
    ELLE_TRACE("file to report: %s", logfile);

    if (!logfile.empty())
    {
      auto std_text = text.toStdString();
      ELLE_DEBUG("user message as std::string: %s", std_text);
      gap_send_user_report(
        this->_state.state(),
        gap_self_email(this->_state.state()),
        std_text.c_str(),
        logfile.c_str(),
        elle::sprintf("%s on %s",
                      common::system::platform(),
                      INFINIT_VERSION).c_str());
      ELLE_DEBUG("report sent");
    }
    else
    {
      ELLE_WARN("Fist: No log file to send");
    }
  }

  this->setFocus();
}

void
InfinitDock::_position_panel()
{
  ELLE_DEBUG_SCOPE("%s: move panel", *this);

  ELLE_DUMP("%s: old position: (%s, %s)", *this, this->x(), this->y());

  QPoint systray_position(this->_systray->geometry().center());

  auto screen = QDesktopWidget().availableGeometry();

  auto x = systray_position.x() - this->width() / 2;
  auto y = systray_position.y() - this->height() / 2;

  static int margin = 5;

  int new_x = qBound(screen.left() + margin, x, screen.right() - margin - this->width());
  int new_y = qBound(screen.top() + margin, y, screen.bottom() - margin - this->height());
  if (this->x() != new_x || this->y() != new_y)
  {
    this->move(new_x, new_y);
    ELLE_DUMP("%s: new position: (%s, %s)", *this, this->x(), this->y());
  }
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
      this->_send_panel->file_adder()->add_file(QUrl::fromLocalFile(file));
    this->_switch_view(this->_send_panel);
    this->show();
  }
}

void
InfinitDock::enterEvent(QEvent* event)
{
  if (this->centralWidget() != nullptr)
    ELLE_DEBUG("%s currently active", *this->centralWidget());
  // if (this->centralWidget() == this->_send_panel)
  //   this->setFocus();
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
  else
    return;

  if (this->centralWidget() == this->_transaction_panel)
  {
    if (event->key() == Qt::Key_Escape)
    {
      ELLE_DEBUG("escape pressed");
      this->toggle_dock(); return;
    }
    else if (event->key() == Qt::Key_S)
    {
      this->_show_send_view(); return;
    }
  }

  QCoreApplication::sendEvent(this->centralWidget(), event);
}

void
InfinitDock::_show_send_view()
{
  ELLE_TRACE_SCOPE("%s: show send view", *this);
  this->_switch_view(this->_send_panel);
}

void
InfinitDock::_show_user_view(uint32_t /* sender_id */)
{
  ELLE_TRACE_SCOPE("%s: show user view", *this);
}

void
InfinitDock::_show_menu()
{
  ELLE_TRACE_SCOPE("%s: show menu", *this);

  this->_menu->show();

  QPoint pos(this->geometry().bottomLeft());

  int margin = 3;
  this->_menu->move(pos.x() + margin,
                    pos.y() - this->_menu->size().height() - margin);

}

void
InfinitDock::_back_from_send_view()
{
  this->_show_transactions_view();
  this->hide();
}

void
InfinitDock::focusOutEvent(QFocusEvent* event)
{
  ELLE_TRACE_SCOPE("%s: focus lost (reason %s)", *this, event->reason());

  // Swallow focus lost event to keep the send view on top
  if (this->centralWidget() == this->_send_panel)
  {
    event->accept();
    return;
  }
  Super::focusOutEvent(event);
  if (event->reason() != Qt::MouseFocusReason)
  {
    this->hide();
  }
}


void
InfinitDock::_show_transactions_view()
{
  ELLE_TRACE_SCOPE("%s: show transaction view", *this);
  this->_switch_view(this->_transaction_panel);
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

  this->_next_panel = panel;
  if (this->centralWidget() != nullptr)
  {
    ELLE_DEBUG("hide %s", *this->centralWidget());
    Panel* current_panel = static_cast<Panel*>(this->centralWidget());
    current_panel->on_hide();
    current_panel->hide();
  }

  this->_activate_new_panel();
}

void
InfinitDock::_activate_new_panel()
{
  Panel* current_panel = static_cast<Panel*>(this->centralWidget());
  if (current_panel != nullptr)
  {
    this->centralWidget()->setParent(0);
  }

  this->setCentralWidget(this->_next_panel);
  this->_next_panel->show();
  this->_next_panel->on_show();

  this->_next_panel = nullptr;
  // XXX: Dirty, but Qt is in trouble to calculate the size of hidden widget.
  // So every time a new panel is shown, make sure that the size matches the
  // content.
  this->update();
  this->adjustSize();
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
InfinitDock::_start_onboarded_reception()
{
  if (this->_onboarder == nullptr)
    this->_onboarder.reset(new fist::onboarding::Onboarder(this));
  QString file = QString::fromStdString(
    elle::os::getenv(
      "FIST_ONBOARDING_FILE",
      (QCoreApplication::applicationDirPath() +
       QDir::separator() + "Infinit - Episode 2.mp4").toStdString()));
  this->_onboarder->receive_file(file);
  connect(this->_onboarder.get(), SIGNAL(reception_completed()),
          this, SLOT(_on_onboarded_reception_completed()));
}

void
InfinitDock::_start_onboarded_sending()
{
  if (this->_onboarder == nullptr)
    this->_onboarder.reset(new fist::onboarding::Onboarder(this));
  this->_onboarder->send_file();
  connect(this->_onboarder.get(), SIGNAL(sending_completed()),
          this, SLOT(_on_onboarded_sending_completed()));
}

void
InfinitDock::_on_onboarded_reception_completed()
{
  ELLE_TRACE_SCOPE("%s: onboarded reception done", *this);
  disconnect(this->_onboarder.get(), SIGNAL(reception_completed()),
             this, SLOT(_on_onboarded_reception_completed()));
  // Store the version to allow to rerun the onboarding on future version by
  // checking the previously stored one.
  fist::settings()["onboarding"].set(
    onboarded_reception_complete, QString(INFINIT_VERSION));

  ELLE_DEBUG("check if onboarded sending has been done")
    if (!fist::settings()["onboarding"].exists(onboarded_sending_complete))
    {
      int delay = 4000;
      ELLE_TRACE_SCOPE("run onboarded sending in %s ms", delay);
      auto* delay_onboarding = new QTimer(this);
      delay_onboarding->setSingleShot(true);
      connect(delay_onboarding, SIGNAL(timeout()),
              this, SLOT(_start_onboarded_sending()));
      delay_onboarding->start(delay);
    }
}

void
InfinitDock::_on_onboarded_sending_completed()
{
  ELLE_TRACE_SCOPE("%s: onboarded sending done", *this);
  disconnect(this->_onboarder.get(), SIGNAL(sending_completed()),
             this, SLOT(_on_onboarded_sending_completed()));
  // Store the version to allow to rerun the onboarding on future version by
  // checking the previously stored one.
  fist::settings()["onboarding"].set(
    onboarded_sending_complete, QString(INFINIT_VERSION));

  this->_onboarder.reset();
}

void
InfinitDock::print(std::ostream& stream) const
{
  stream << "Dock";
}
