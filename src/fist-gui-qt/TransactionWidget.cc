#include <map>

#include <QEvent>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionWindow.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionWidget");

QVector<gap_TransactionStatus> g_finals =
{
  gap_transaction_finished,
  gap_transaction_failed,
  gap_transaction_canceled,
  gap_transaction_rejected,
  gap_transaction_cloud_buffered,
};

TransactionWidget::TransactionWidget(fist::model::Transaction const& model):
  ListItem(nullptr, view::background, false),
  _transaction(model),
  _peer_avatar(new AvatarWidget(this->_transaction.avatar())),
  _peer_status(new QLabel),
  _layout(nullptr),
  _filename(new QLabel(this)),
  _accept_button(new IconButton(QPixmap(":/buttons/accept.png"))),
  _reject_button(new IconButton(QPixmap(":/buttons/reject.png"))),
  _accept_reject_area(new QWidget),
  _cancel_button(new IconButton(QPixmap(":/buttons/cancel.png"))),
  _mtime(new QLabel),
  _status(new QLabel),
  _info_area(new QWidget),
  _progress_timer(nullptr),
  _mtime_updater(new QTimer(this))
{
  connect(&this->_transaction, SIGNAL(avatar_updated()),
          this, SLOT(_on_avatar_updated()));
  connect(&this->_transaction, SIGNAL(status_updated()),
          this, SLOT(apply_update()));

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
    this->_peer_fullname = username;
    user_and_status->addWidget(this->_peer_status, 0, Qt::AlignLeft);
    this->_peer_status->setToolTip(tr("Online"));
    user_and_status->addStretch(0);
    texts->addSpacing(4);

    {
      this->_filename->setText(
        this->_transaction.files().size() == 1 ?
        this->_transaction.files().first() :
        QString("%1 files").arg(this->_transaction.files().size()));

      this->_filename->installEventFilter(this);
      this->_filename->setToolTip(this->_transaction.tooltip());
      view::transaction::files::style(*this->_filename);
      this->_filename->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
      texts->addWidget(this->_filename);
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

    this->_accept_button->setToolTip("Accept");
    this->_reject_button->setToolTip("Reject");

    layout->addWidget(this->_accept_reject_area);
  }

  setSizePolicy(QSizePolicy::Minimum,
                QSizePolicy::Minimum);
  adjustSize();

#if 0
  connect(this->_peer_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
#endif

  this->apply_update();
  this->update();
}

bool
TransactionWidget::eventFilter(QObject *obj, QEvent *event)
{
  if (!dynamic_cast<QWidget*>(obj))
    return Super::eventFilter(obj, event);

  if (obj == this->_filename)
  {
    if (!this->_transaction.is_sender() && this->_transaction.status() == gap_transaction_finished)
    {
      if (event->type() == QEvent::Enter)
      {
        view::transaction::files::hover_style(*this->_filename);
        this->update();
      }
      else if (event->type() == QEvent::Leave)
      {
        view::transaction::files::style(*this->_filename);
        this->update();
      }
      else if (event->type() == QEvent::MouseButtonPress)
        emit open_file(this->_transaction.id());
    }
  }

  return Super::eventFilter(obj, event);
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
  return QSize(Super::sizeHint().width(), size.height());
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
TransactionWidget::_on_avatar_updated()
{
  this->_peer_avatar->setPicture(this->_transaction.avatar());
}

void
TransactionWidget::apply_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);
  this->_info_area->show();
  this->_cancel_button->hide();
  this->_accept_reject_area->hide();

  ELLE_DEBUG("peer is %sconnected",
             this->_transaction.peer_connection_status() ? "" : "dis");
  if (this->_transaction.peer_connection_status())
    this->_peer_status->show();
  else
    this->_peer_status->hide();

  if (this->_transaction.status() == gap_transaction_waiting_accept &&
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

  if (this->_transaction.status() == gap_transaction_transferring &&
      this->_progress_timer == nullptr)
  {
    ELLE_TRACE("run progress timer");
    this->_peer_avatar->setTransactionCount(this->_transaction.files().size());

    this->_progress_timer.reset(new QTimer);
    connect(this->_progress_timer.get(), SIGNAL(timeout()),
            this, SLOT(update_progress()));
    this->_progress_timer->start(AvatarWidget::progress_update_interval);

    connect(this,
            SIGNAL(onProgressChanged(float)),
            this->_peer_avatar,
            SLOT(setProgress(float)));
  }
  else if (this->_transaction.status() != gap_transaction_transferring &&
           this->_progress_timer != nullptr)
  {
    setProgress(0);
    this->_progress_timer.reset();
  }

  if (g_finals.contains(this->_transaction.status()))
  {
    this->_peer_avatar->setTransactionCount(0);
  }

  this->_on_status_updated();
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
  emit transaction_accepted(this->_transaction.id());
}

void
TransactionWidget::reject()
{
  ELLE_TRACE_SCOPE("%s: reject transaction", *this);
  emit transaction_rejected(this->_transaction.id());
}

void
TransactionWidget::cancel()
{
  ELLE_TRACE_SCOPE("%s: cancel transaction", *this);

  emit transaction_canceled(this->_transaction.id());
}

/*-------.
| Status |
`-------*/
void
TransactionWidget::_on_status_updated()
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
    { gap_transaction_new, StatusUpdater(QString(), false, "New") },
    { gap_transaction_waiting_accept,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Wait for user to accept") },
    { gap_transaction_waiting_data,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Waiting for data") },
    { gap_transaction_connecting,
        StatusUpdater(QString(":/icons/loading.gif"),
                      true,
                      "Connecting") },
    { gap_transaction_transferring,
        StatusUpdater(QString(), false, "Transferring") },
    { gap_transaction_finished,
        StatusUpdater(QString(":/icons/check.png"), false, "Finished") },
    { gap_transaction_cloud_buffered,
        StatusUpdater(QString(":/icons/check.png"), false, "Cloud Buffered") },
    { gap_transaction_failed,
        StatusUpdater(QString(":/icons/error.png"), false, "Failed") },
    { gap_transaction_canceled,
        StatusUpdater(QString(":/icons/error.png"), false, "Canceled") },
    { gap_transaction_rejected,
        StatusUpdater(QString(":/icons/error.png"), false, "Rejected") },
  };

  if (this->_transaction.status() == gap_transaction_finished)
    emit transaction_finished(this->_transaction.id());

  tooltips.at(this->_transaction.status())(*this->_status);
  if (this->_transaction.status() == gap_transaction_transferring)
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

  if (this->_peer_fullname->text().isEmpty())
    this->_peer_fullname->setText(this->_transaction.peer_fullname());
}

static
int
seconds_since_midnight(QDateTime const& date)
{
  auto time = date.time();
  return time.hour() * 3600 + time.minute() * 60 + time.second();
}
void
TransactionWidget::update_mtime()
{
  ELLE_DUMP_SCOPE("mtime updated");
  auto current = QDateTime::currentDateTime();
  auto transaction_mtime = this->_transaction.mtime().toLocalTime();
  QString format(transaction_mtime.secsTo(current) < seconds_since_midnight(current)
                 ? "h:mA"
                 : "MMM d");
  this->_mtime->setText(transaction_mtime.toString(format));
}

void
TransactionWidget::print(std::ostream& stream) const
{
  stream << "TransactionWidget(" << this->_transaction << ")";
}
