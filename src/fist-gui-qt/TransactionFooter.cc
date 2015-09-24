# include <QHBoxLayout>
# include <QSpacerItem>
# include <QEvent>

# include <elle/log.hh>

# include <fist-gui-qt/TransactionFooter.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/utils/printers.hh>
# include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("fist.TransactionFooter");

namespace
{
  static
  fist::style::Text const
  style(fist::Font(Arial, 9), QColor(0xFF, 0xFF, 0xFF), Qt::AlignCenter);
}

TransactionFooter::TransactionFooter(fist::State& state)
  : Footer(nullptr)
  , _state(state)
  , _menu(new IconButton(":/main/gear", this))
  , _usage_bar(new QProgressBar(this))
  , _usage_caption(new QLabel(this))
  , _send(new IconButton(":/main/transfer", this))
{
this->_layout->setContentsMargins(7, 7, 7, 0);
  this->_usage_bar->setMinimum(0);
  this->_usage_bar->setTextVisible(false);
  this->_usage_bar->setStyleSheet(
    "QProgressBar {"
    "  background-color: #F8AFAD;"
    "  border: 0px;"
    "  border-radius: 2px;"
    "}"
    "QProgressBar::chunk {"
    "  background-color: #FFFFFF;"
    "  border: 0px;"
    "  border-radius: 2px;"
    "}"
    "QProgressBar::chunk:horizontal {"
    "background: qlineargradient(x1: 0,"
    "                            y1: 0,"
    "                            x2: 0.2,"
    "                            y2: 0,"
    "                            stop: 0 white,"
    "                            stop: 1 white);"
    "}"
    );
  this->_usage_bar->setFixedHeight(6);
  this->_usage_bar->installEventFilter(this);
  ::style(*this->_usage_caption);
  this->_usage_caption->installEventFilter(this);
  this->_usage_bar->setToolTip("Click to discover how to get more");
  this->_usage_caption->setToolTip("Click to discover how to get more");
  this->_menu->setToolTip("Options");
  this->_send->setToolTip("Send files");
  this->_layout->addWidget(this->_menu, 0, Qt::AlignCenter);
  this->_layout->addStretch();
  QVBoxLayout* usage = new QVBoxLayout;
  usage->setContentsMargins(0, 1, 0, 1);
  usage->setSpacing(2);
  usage->addWidget(this->_usage_bar, 0, Qt::AlignHCenter | Qt::AlignTop);
  usage->addStretch();
  usage->addWidget(this->_usage_caption, 0, Qt::AlignHCenter | Qt::AlignBottom);
  this->_layout->addLayout(usage);
  this->_layout->addStretch();
  this->_layout->addWidget(this->_send, 0, Qt::AlignCenter);

  connect(&this->_state, SIGNAL(account_updated()),
          this, SLOT(_account_updated()));

  this->_mode_implementation();
}

void
TransactionFooter::_account_updated()
{
  this->_usage_caption->show();
  this->_usage_bar->show();
  switch (this->mode())
  {
    case fist::Mode::p2p:
      if (this->_state.account().quotas.value().send_to_self.quota)
      {
        auto const& sts = this->_state.account().quotas.value().send_to_self;
        auto remaining = sts.quota.get() - sts.used;
        this->_usage_caption->setText(
          QString_from_utf8_string(
            elle::sprintf("%s monthly transfers to yourself left", remaining)));
        this->_usage_bar->setMaximum(sts.quota.get());
        this->_usage_bar->setValue(sts.quota.get() - remaining);
      }
      else
      {
        this->_usage_caption->hide();
        this->_usage_bar->hide();
      }
      break;
    case fist::Mode::link:
    {
      auto const& links = this->_state.account().quotas.value().links;
      int64_t quota = links.quota.get();
      // Not cool...
      int64_t remaining = std::max((int64_t) 0, quota - (int64_t) links.used);
      // To make sure we
      double remaining_ratio = 1.0 * remaining / quota;
      ELLE_DEBUG("quota: %s", quota);
      ELLE_DEBUG("remaining: %s", remaining);
      ELLE_DEBUG("remaining ratio: %s", remaining_ratio);
      this->_usage_caption->setText(
        QString_from_utf8_string(
          elle::sprintf("%s storage left", readable_size(remaining))));
      this->_usage_bar->setMaximum(100);
      this->_usage_bar->setValue(100 * (1.0 - remaining_ratio));
    }
  default:
    break;
  }
}

void
TransactionFooter::_mode_implementation()
{
  switch (this->mode())
  {
    case fist::Mode::p2p:
      this->_send->set_pixmap(":/send/transfer");
      this->_send->setToolTip("Send");
      this->update();
      break;
    case fist::Mode::link:
      this->_send->set_pixmap(":/send/upload");
      this->_send->setToolTip("Generate a link");
      this->update();
      break;
  }
  this->_account_updated();
}

bool
TransactionFooter::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == this->_usage_caption || this->_usage_bar)
  {
    if (event->type() == QEvent::Enter)
    {
      this->setCursor(QCursor(Qt::PointingHandCursor));
    }
    else if (event->type() == QEvent::Leave)
    {
      this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (event->type() == QEvent::MouseButtonRelease)
    {
      this->setCursor(QCursor(Qt::ArrowCursor));
      this->_state.go_to_online_profile(
        QString_from_utf8_string(
          elle::sprintf("panel_%s",
                        // Write a working pretty printer.
                        this->mode() == fist::Mode::p2p
                        ? std::string("p2p")
                        : std::string("link"))));
    }
  }
  return Super::eventFilter(obj, event);
}
