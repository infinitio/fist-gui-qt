#include <map>

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include <elle/log.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionWindow.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionWidget");

QVector<gap_TransactionStatus> g_finals =
{
  gap_transaction_cleaning,
  gap_transaction_finished,
  gap_transaction_failed,
  gap_transaction_canceled,
  gap_transaction_rejected,
};

TransactionWidget::TransactionWidget(TransactionModel const& model):
  ListItem(nullptr, view::background, false),
  _transaction(model),
  _peer_avatar(new AvatarWidget(this->_transaction.avatar())),
  _peer_status(new QLabel),
  _layout(nullptr),
  _accept_button(new IconButton(QPixmap(":/buttons/accept.png"))),
  _reject_button(new IconButton(QPixmap(":/buttons/reject.png"))),
  _accept_reject_area(new QWidget),
  _cancel_button(new IconButton(QPixmap(":/buttons/cancel.png"))),
  _mtime(new QLabel),
  _status(new QLabel),
  _info_area(new QWidget),
  _timer(nullptr)
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);

  this->_peer_status->setPixmap(QPixmap(":/icons/status.png"));

  connect(this->_accept_button, SIGNAL(released()),
          this, SLOT(accept()));
  connect(this->_reject_button, SIGNAL(released()),
          this, SLOT(reject()));
  connect(this->_cancel_button, SIGNAL(released()),
          this, SLOT(cancel()));

  this->_peer_avatar = new AvatarWidget(this->_transaction.avatar());

  auto layout = new QHBoxLayout(this);
  // XXX: should but 13, 13, 13, 13 but avatar widget size is strange.
  layout->setContentsMargins(8, 8, 13, 8);
  this->_layout = layout;
  layout->addWidget(this->_peer_avatar, 0, Qt::AlignLeft);

  {
    auto texts = new QVBoxLayout;
    texts->setContentsMargins(5, 0, 5, 0);

    layout->addLayout(texts);

    texts->addStretch();
    auto user_and_status = new QHBoxLayout;
    texts->addLayout(user_and_status);
    auto username = new QLabel(this->_transaction.peer_fullname());
    {
      view::transaction::peer::style(*username);
      username->setMaximumWidth(150);
      username->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
      user_and_status->addWidget(username);
    }
     user_and_status->addWidget(this->_peer_status, 0, Qt::AlignLeft);
    this->_peer_status->setToolTip(tr("Online"));
    user_and_status->addStretch(0);
    texts->addSpacing(4);

    auto filename = this->_transaction.files().size() == 1 ?
      new QLabel(this->_transaction.files().first()) :
      new QLabel(QString("%1 files").arg(this->_transaction.files().size()));
    {
      filename->setToolTip(this->_transaction.tooltip());
      view::transaction::files::style(*filename);
      filename->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
      texts->addWidget(filename);
    }
    texts->addStretch();
  }
  layout->addStretch();
  {
    auto time_and_info = new QVBoxLayout;
    this->_info_area->setLayout(time_and_info);
    view::transaction::date::style(*this->_mtime);
    time_and_info->addWidget(this->_mtime, 0, Qt::AlignCenter | Qt::AlignRight);
    auto status_and_cancel = new QHBoxLayout;
    time_and_info->addLayout(status_and_cancel);
    {
      status_and_cancel->addStretch();
      status_and_cancel->addWidget(this->_status, 0, Qt::AlignCenter | Qt::AlignRight);
      status_and_cancel->addSpacing(3);
      status_and_cancel->addWidget(this->_cancel_button, 0, Qt::AlignCenter | Qt::AlignRight);
    }
    layout->addWidget(this->_info_area);
  }
  {
    auto infos = new QVBoxLayout;
    this->_accept_reject_area->setLayout(infos);

    infos->addWidget(this->_accept_button, 0, Qt::AlignCenter | Qt::AlignLeft);
    infos->addWidget(this->_reject_button, 0, Qt::AlignCenter | Qt::AlignLeft);

    layout->addWidget(this->_accept_reject_area);
  }

  this->_update();

  setSizePolicy(QSizePolicy::Minimum,
                QSizePolicy::Minimum);
  adjustSize();

#if 0
  connect(this->_peer_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
#endif

  auto mtime_updater = new QTimer(this);
  connect(mtime_updater, SIGNAL(timeout()),
          this, SLOT(update_mtime()));
  mtime_updater->start(5000);
  this->update();
}

/*-----------.
| Properties |
`-----------*/

float
TransactionWidget::progress() const
{
  return this->_peer_avatar->progress();
}

void
TransactionWidget::setProgress(float value)
{
  ELLE_DEBUG("%s: update progress to %s", *this, value);
  this->_peer_avatar->setProgress(value);
}

/*-------.
| Layout |
`-------*/

QSize
TransactionWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(this->widthHint(), size.height());
}

QSize
TransactionWidget::minimumSizeHint() const
{
  return this->sizeHint();
}

void
TransactionWidget::trigger()
{
  ELLE_TRACE_SCOPE("%s: clicked", *this);
#if 0
  auto pop = new TransactionWindow(parentWidget());
  parentWidget()->hide();

  pop->show();
  pop->setFocus();
#endif
}

void
TransactionWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);
  this->_info_area->show();
  this->_cancel_button->hide();
  this->_accept_reject_area->hide();

  if (this->_transaction.new_avatar())
  {
    ELLE_DEBUG("new avatar");
    this->_peer_avatar->setPicture(this->_transaction.avatar());
  }

  ELLE_DEBUG("peer is %sconnected",
             this->_transaction.peer_connection_status() ? "" : "dis");
  if (this->_transaction.peer_connection_status())
    this->_peer_status->show();
  else
    this->_peer_status->hide();

  if (this->_transaction.status() == gap_transaction_waiting_for_accept &&
      !this->_transaction.is_sender())
  {
    ELLE_DEBUG("show accept / reject buttons");
    this->_accept_reject_area->show();
    this->_info_area->show();
    this->_mtime->hide();
  }
  else if (!this->_accept_reject_area->isHidden())
  {
    ELLE_DEBUG("hide accept / reject buttons");
    this->_accept_reject_area->hide();
    this->_mtime->show();
    this->_info_area->show();
  }

  if (this->_accept_reject_area->isHidden())
  {
    if (!g_finals.contains(this->_transaction.status()))
    {
      this->_cancel_button->show();
    }
    this->_status->show();
    this->_mtime->show();
    this->_info_area->show();
  }
  else
  {
     this->_info_area->hide();
  }

  if (this->_transaction.status() == gap_transaction_running &&
      this->_timer == nullptr)
  {
    ELLE_TRACE("run progress timer");
    this->_peer_avatar->setTransactionCount(this->_transaction.files().size());

    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()), this, SLOT(update_progress()));
    _timer->start(1000);

    connect(this,
            SIGNAL(onProgressChanged(float)),
            this->_peer_avatar,
            SLOT(setProgress(float)));
  }
  else if (this->_transaction.status() != gap_transaction_running &&
           this->_timer != nullptr)
  {
    setProgress(0);
    delete this->_timer;
    this->_timer = nullptr;
  }

  if (g_finals.contains(this->_transaction.status()))
  {
    this->_peer_avatar->setTransactionCount(0);
  }

  this->update_status();
}

void
TransactionWidget::update_progress()
{
  ELLE_DUMP("%s: update progress", *this);
  float progress = this->_transaction.progress();
  emit onProgressChanged(progress);
}

void
TransactionWidget::accept()
{
  ELLE_TRACE_SCOPE("%s: accept transaction", *this);
  emit on_transaction_accepted(this->_transaction.id());
}

void
TransactionWidget::reject()
{
  ELLE_TRACE_SCOPE("%s: reject transaction", *this);
  emit on_transaction_rejected(this->_transaction.id());
}

void
TransactionWidget::cancel()
{
  ELLE_TRACE_SCOPE("%s: cancel transaction", *this);

  emit on_transaction_canceled(this->_transaction.id());
}

static
QString
QDateTime_to_friendly_duration(QDateTime const& time)
{
  auto secs = time.secsTo(QDateTime::currentDateTimeUtc());
  std::vector<std::pair<int, QString>> printers{
    {86400, "day"}, {3600, "hour"}, {60, "min"}, {1, "sec"}};

  for (auto const& duration_pair: printers)
    if (secs > duration_pair.first)
      return QString::fromStdString(
        elle::sprintf("%s %s%s ago",
                      secs / duration_pair.first,
                      duration_pair.second,
                      ((secs / duration_pair.first) > 1) ? "s" : ""));
  return "...";
}

/*-------.
| Status |
`-------*/
void
TransactionWidget::update_status()
{
  struct StatusUpdater:
    public elle::Printable
  {
    StatusUpdater(QString const& image_path,
                  bool animated,
                  QString const& tooltip):
      _image_path(image_path),
      _animated(animated),
      _tooltip(tooltip)
    {}

    void
    operator () (QLabel& label) const
    {
      ELLE_TRACE_SCOPE("%s: update label", *this);

      label.setToolTip(this->_tooltip);
      if (this->_animated)
      {
        label.setMovie(new QMovie(this->_image_path));
        label.movie()->start();
      }
      else
      {
        label.setPixmap(QPixmap(this->_image_path));
      }
    }

  private:
    QString _image_path;
    bool _animated;
    QString _tooltip;

    void
    print(std::ostream& stream) const override
    {
      stream << "StatusUpdater";
    }

  };

  static std::map<gap_TransactionStatus, StatusUpdater> tooltips{
    { gap_transaction_none, StatusUpdater(QString(), false, "None") },
    { gap_transaction_pending, StatusUpdater(QString(), false, "Pending") },
    { gap_transaction_copying, StatusUpdater(QString(), false, "Copying") },
    { gap_transaction_waiting_for_accept,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Wait for user to accept") },
    { gap_transaction_accepted,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Waiting for peer to be online") },
    { gap_transaction_preparing,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Waiting for peer to be online") },
    { gap_transaction_running, StatusUpdater(QString(), false, "Running") },
    { gap_transaction_cleaning,
        StatusUpdater(QString(), false, "Cleaning") },
    { gap_transaction_finished,
        StatusUpdater(QString(":/icons/check.png"), false, "Finished") },
    { gap_transaction_failed,
        StatusUpdater(QString(":/icons/error.png"), false, "Failed") },
    { gap_transaction_canceled,
        StatusUpdater(QString(":/icons/error.png"), false, "Canceled") },
    { gap_transaction_rejected,
        StatusUpdater(QString(":/icons/error.png"), false, "Rejected") },
  };

  tooltips.at(this->_transaction.status())(*this->_status);
  if (this->_transaction.status() == gap_transaction_running)
  {
    if (this->_transaction.is_sender())
    {
      this->_status->setPixmap(QPixmap(QString(":/icons/main-upload.png")));
      this->_status->setToolTip(QString("Uploading"));
    }
    else
    {
      this->_status->setPixmap(QPixmap(QString(":/icons/main-download.png")));
      this->_status->setToolTip(QString("Downloading"));
    }
  }

  this->update_mtime();
}

void
TransactionWidget::update_mtime()
{
  ELLE_DUMP_SCOPE("mtime updated");
  this->_mtime->setText(
    QDateTime_to_friendly_duration(this->_transaction.mtime()));
}

void
TransactionWidget::print(std::ostream& stream) const
{
  stream << "TransactionWidget(" << this->_transaction << ")";
}
