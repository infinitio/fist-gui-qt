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

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/os/environ.hh>

#include <version.hh>
#include <common/common.hh>
#include <surface/gap/gap.hh>

#include <fist-gui-qt/InfinitDock.hh>
#include <fist-gui-qt/RoundShadowWidget.hh>
#include <fist-gui-qt/SendView/Panel.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/utils.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/icons.hh>
#include <fist-gui-qt/Settings.hh>
#include <fist-gui-qt/onboarding/Onboarder.hh>

ELLE_LOG_COMPONENT("infinit.FIST.Dock");

static const QString
onboarded_reception_complete("reception_complete");
static const QString
onboarded_sending_complete("sending_complete");

// This ugly macro prevent method to be called multiple times.
#define ENSURE_ONE_AT_A_TIME()                                          \
  static bool __tag__ = false;                                          \
  if (__tag__ == true)                                                  \
    return;                                                             \
  __tag__ = true;                                                       \
  elle::SafeFinally release_reporting([] { __tag__ = false; }); /**/

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
    gap_user_status_callback(state, InfinitDock::user_status_cb);
    gap_avatar_available_callback(state, InfinitDock::avatar_available_cb);
  }
};

/*-------------.
| Construction |
`-------------*/
// Creating the transaction panel is a long operation. So we just wait until all
// the graphical part is fully load, and then, initialize it.
InfinitDock::InfinitDock(fist::State& state,
                         fist::gui::systray::Icon& systray)
  : _prologue(new Prologue(state.state()))
  , _state(state)
  , _transaction_panel(nullptr)
  , _send_panel(new fist::sendview::Panel(this->_state))
  , _next_panel(nullptr)
  , _menu(new QMenu(this))
  , _logo(":/menu-bar/fire@2x")
  , _systray(systray)
  , _systray_menu(new QMenu(this))
  , _show(new QAction(tr("&Show dock"), this))
  , _send_files(new QAction(tr("&Send files..."), this))
  , _report_a_problem(new QAction(tr("&Report a problem"), this))
  , _logout(new QAction(tr("&Logout"), this))
  , _quit(new QAction(tr("&Quit"), this))
  , _update(nullptr)
#ifndef FIST_PRODUCTION_BUILD
  , _start_onboarding_action(new QAction(tr("&Start onboarding"), this))
#endif
{
  g_dock = this;
  this->_systray.set_icon(fist::icon::normal);
  this->_systray.show();
  // System Tray.
  {
    this->_systray_menu->addAction(_show);
    this->_systray_menu->addAction(_send_files);
    this->_systray_menu->addAction(_quit);
    this->_systray.inner()->setContextMenu(_systray_menu);
    connect(this->_systray.inner(),
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(_systray_activated(QSystemTrayIcon::ActivationReason)));
    connect(this->_systray.inner(), SIGNAL(messageClicked()),
            this, SLOT(_systray_message_clicked()));
  }


  this->_transaction_panel.reset(new MainPanel(this->_state));

  this->_register_panel(this->_transaction_panel.get());
  this->_register_panel(this->_send_panel.get());

  connect(this, SIGNAL(onSizeChanged()),
          SLOT(_position_panel()));

  {
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

    connect(this->_send_panel.get(),
            SIGNAL(switch_signal()),
            this,
            SLOT(_back_from_send_view()));

    connect(this->_send_panel.get(),
            SIGNAL(choose_files()),
            this,
            SLOT(pick_files()));
  }

  connect(this,
          SIGNAL(avatar_available(uint32_t)),
          this->_send_panel.get(),
          SLOT(avatar_available(uint32_t)));

  // XXX: Specialize a QWidgetAction to add a better visual and for example,
  // to copy the version in the user clipboard on click.
  QWidgetAction* version = new QWidgetAction(this);
  auto* v = new QLabel(QString(INFINIT_VERSION));
  view::version_style(*v);
  version->setDefaultWidget(v);
  this->_menu->addAction(version);
  this->_menu->addSeparator();
  this->_menu->addAction(_report_a_problem);
  this->_menu->addSeparator();
  this->_menu->addAction(_logout);
  this->_menu->addAction(_quit);

  // Register gap callback.
  gap_user_status_callback(_state.state(), InfinitDock::user_status_cb);
  gap_avatar_available_callback(_state.state(), InfinitDock::avatar_available_cb);

  this->_show_transactions_view();

  this->connect(_send_files, SIGNAL(triggered()), this, SLOT(pick_files()));
  this->connect(_show, SIGNAL(triggered()), this, SLOT(show()));
  this->connect(_report_a_problem, SIGNAL(triggered()),
                this, SLOT(report_a_problem()));
  this->connect(_logout, SIGNAL(triggered()), this, SLOT(_on_logout()));
  this->connect(_logout, SIGNAL(triggered()), this, SLOT(hide()));
  this->connect(_logout, SIGNAL(triggered()), this, SIGNAL(logout_request()));
  this->connect(_quit, SIGNAL(triggered()), this, SLOT(_on_logout()));
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

  connect(&this->_state, SIGNAL(acceptable_transactions_changed(size_t)),
          this, SLOT(_active_transactions_changed(size_t)));
  connect(&this->_state, SIGNAL(running_transactions_changed(size_t)),
          this, SLOT(_active_transactions_changed(size_t)));

  this->hide();

#ifndef FIST_PRODUCTION_BUILD
  this->_menu->addAction(_start_onboarding_action);
  this->connect(this->_start_onboarding_action, SIGNAL(triggered()),
                this, SLOT(_start_onboarded_reception()));
#endif
}

/*------------.
| Destruction |
`------------*/
InfinitDock::~InfinitDock()
{
  ELLE_TRACE_SCOPE("%s: quit", *this);
  this->_on_logout();
}

void
InfinitDock::_on_logout()
{
  ELLE_TRACE_SCOPE("%s: logout", *this);
  // Kill everything in order to make sure nothing requiring state is running
  // when destroying it.
  this->setCentralWidget(nullptr);
  this->_send_panel.reset();
  this->_transaction_panel.reset();
}

/*----------------.
| State Callbacks |
`----------------*/
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

/*------------.
| System Tray |
`------------*/
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
InfinitDock::_systray_message(fist::SystrayMessageCarrier const& message)
{
  this->_last_message = std::move(message.body);
  auto const& body = *this->_last_message;
  ELLE_TRACE_SCOPE("%s: show system tray message: %s - %s",
                   *this, body.title(), body.body());
  if (body.always_show() || !this->isVisible())
    this->_systray.inner()->showMessage(
      body.title(), body.body(), body.icon(), body.duration());
}

void
InfinitDock::_systray_message_clicked()
{
  ELLE_TRACE_SCOPE("%s: message clicked", *this);
  elle::SafeFinally show([&] { this->show(); });
  if (this->_last_message == nullptr)
  {
    ELLE_WARN("%s: no systray message stored", *this);
  }
  else
  {
    if (dynamic_cast<fist::UpdateAvailableMessage const*>(
          this->_last_message.get()))
    {
      emit this->update_application();
    }
    this->_last_message.reset();
  }
}

void
InfinitDock::_active_transactions_changed(size_t)
{
  bool acceptable = (this->_state.acceptable_transactions() != 0);
  bool running = (this->_state.running_transactions() != 0);

  if (acceptable)
  {
    if (running)
      this->_systray.set_icon(fist::icon::transferring_waiting_for_decistion);
    else
      this->_systray.set_icon(fist::icon::waiting_for_decistion);
  }
  else
  {
    if (running)
      this->_systray.set_icon(fist::icon::transferring_waiting_for_decistion);
    else
      this->_systray.set_icon(fist::icon::normal);
  }
}

/*-------.
| Update |
`-------*/
void
InfinitDock::update_available(bool mandatory,
                              QString const& changelog)
{
}

void
InfinitDock::download_progress(qint64 downloaded, qint64 total_size)
{
}

void
InfinitDock::download_ready()
{
  ELLE_TRACE_SCOPE("%s: an update is available", *this);
  this->_systray_message(
    fist::SystrayMessageCarrier(
      new fist::UpdateAvailableMessage(
        "Update!", "An update is available, click to automatically update")));
  if (this->_update == nullptr)
  {
    this->_menu->addSeparator();
    this->_update = new QAction("Update", this);
    connect(this->_update, SIGNAL(triggered()), this, SIGNAL(update_application()));
    this->_menu->addAction(this->_update);
  }
}

/*------.
| Panel |
`------*/
void
InfinitDock::_register_panel(Panel* panel)
{
  ELLE_ASSERT(panel != nullptr);

  ELLE_TRACE_SCOPE("%s: register panel %s", *this, *panel);
  connect(panel, SIGNAL(systray_message(fist::SystrayMessageCarrier const&)),
          this, SLOT(_systray_message(fist::SystrayMessageCarrier const&)));
  connect(panel, SIGNAL(set_background_color(QColor const&)),
          this, SLOT(setBackground(QColor const&)));
  panel->hide();
}

MainPanel&
InfinitDock::transactionPanel()
{
  return *this->_transaction_panel;
}

fist::sendview::Panel&
InfinitDock::send_panel() const
{
  return *this->_send_panel;
}

void
InfinitDock::_position_panel()
{
  ELLE_DEBUG_SCOPE("%s: move panel", *this);

  ELLE_DUMP("%s: old position: (%s, %s)", *this, this->x(), this->y());

  QPoint systray_position(this->_systray.inner()->geometry().center());

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
InfinitDock::_show_send_view()
{
  ELLE_TRACE_SCOPE("%s: show send view", *this);
  this->_switch_view(this->_send_panel.get());
}

void
InfinitDock::_show_user_view(uint32_t /* sender_id */)
{
  ELLE_TRACE_SCOPE("%s: show user view", *this);
}

void
InfinitDock::_show_transactions_view()
{
  ELLE_TRACE_SCOPE("%s: show transaction view", *this);
  this->_switch_view(this->_transaction_panel.get());
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
InfinitDock::_back_from_send_view()
{
  this->_show_transactions_view();
  this->hide();
}

/*-----------.
| Visibility |
`-----------*/
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
InfinitDock::showEvent(QShowEvent* event)
{
  ELLE_LOG_SCOPE("%s: show dock", *this);

  this->updateGeometry();
  this->update();
  this->adjustSize();
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
    this->_systray_message(
      fist::SystrayMessageCarrier(new fist::Message(
        "Infinit is minimized!",
        "Make sure the Infinit icon is always "
        "visible by clicking customize!",
        QSystemTrayIcon::Information,
        60000)));
  }
}

/*------.
| Focus |
`------*/
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
InfinitDock::focusOutEvent(QFocusEvent* event)
{
  ELLE_TRACE_SCOPE("%s: focus lost (reason %s)", *this, event->reason());
  // Swallow focus lost event to keep the send view on top
  if (this->centralWidget() == this->_send_panel.get())
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

/*-----------------------------.
| Mouse / Keyboard interaction |
`-----------------------------*/
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
InfinitDock::keyPressEvent(QKeyEvent* event)
{
  ELLE_TRACE_SCOPE("%s: key pressed (%s)", *this, event->key());

  if (this->centralWidget() != nullptr)
    ELLE_DEBUG("%s currently active", *this->centralWidget());
  else
    return;

  if (this->centralWidget() == this->_transaction_panel.get())
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

/*------.
| Other |
`------*/
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

// XXX: Qt file dialog implementation is really broken.
// You have two ways to spawn a file dialog:
// - Instantiating a QFileDialog, which would be the optimal solution because
//   you get full control over lifetime, signals / slots, automatic current
//   directory update... But it's not a native one (basically it's ugly).
// - Use the static methods, which give you a native window but you don't have
//   any control on it (e.g. force quit) from the code.
void
InfinitDock::pick_files()
{
  ENSURE_ONE_AT_A_TIME();

  ELLE_TRACE_SCOPE("%s: spawn file picker", *this);

  QStringList selected = QFileDialog::getOpenFileNames(
    this,
    tr("Select files to send"));

  if (selected.size())
  {
    for (auto const& file: selected)
      this->_send_panel->file_adder()->add_file(QUrl::fromLocalFile(file));
    this->_switch_view(this->_send_panel.get());
    this->show();
  }
}

void
InfinitDock::report_a_problem()
{
  ENSURE_ONE_AT_A_TIME();

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
      boost::filesystem::path home(common::infinit::home());
      boost::filesystem::path copied_log = home / "current_state.log";
      boost::system::error_code erc;
      boost::filesystem::copy(logfile, copied_log , erc);
      if (erc)
      {
        ELLE_TRACE("error while copying %s: %s", logfile, copied_log);
      }
      elle::SafeFinally cleanup{
        [&] {
          boost::system::error_code erc;
          boost::filesystem::remove(copied_log, erc);
          if (erc)
            ELLE_WARN("removing copied file %s failed: %s", copied_log, erc);
        }};
      auto array = text.toUtf8();
      std::string std_text(array.constData());
      ELLE_DEBUG("user message as std::string: %s", std_text);
      gap_send_user_report(
        this->_state.state(),
        gap_self_email(this->_state.state()),
        std_text.c_str(),
        copied_log.string().c_str());
      ELLE_DEBUG("report sent");
    }
    else
    {
      ELLE_WARN("Fist: No log file to send");
    }
  }

  this->setFocus();
}

/*-----------.
| Onboarding |
`-----------*/
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

/*------.
| Print |
`------*/
void
InfinitDock::print(std::ostream& stream) const
{
  stream << "Dock";
}
