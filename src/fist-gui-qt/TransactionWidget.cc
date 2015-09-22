#include <map>

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QSpacerItem>

#include <elle/log.hh>

#include <surface/gap/gap.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionWindow.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/model/User.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionWidget");

TransactionWidget::TransactionWidget(Model const& model):
  ListItem(nullptr, view::background, false),
  _transaction(model),
  _peer_avatar(new AvatarWidget(this->_transaction.peer().avatar(), this)),
  _peer_status(new QLabel(this)),
  _layout(nullptr),
  _filename(new QLabel(this)),
  _accept_button(new IconButton(":/conversation/accept", this)),
  _reject_button(new IconButton(":/conversation/reject", this)),
  _accept_reject_area(new QWidget(this)),
  _pause_button(new IconButton(":/conversation/pause", this)),
  _cancel_button(new IconButton(":/conversation/cancel", this)),
  _mtime(new QLabel(this)),
  _status(new QLabel(this)),
  _info_area(new QWidget(this)),
  _progress_timer(nullptr)
{
  connect(&this->_transaction, SIGNAL(status_updated()),
          this, SLOT(apply_update()));
  connect(&this->_transaction, SIGNAL(peer_changed()),
          this, SLOT(_update_peer()));
  this->_connect_peer();

  ELLE_TRACE_SCOPE("%s: contruction", *this);

  this->_peer_status->setPixmap(QPixmap(":/conversation/peer-online"));

  connect(this->_accept_button, SIGNAL(released()),
          this, SLOT(accept()));
  connect(this->_reject_button, SIGNAL(released()),
          this, SLOT(reject()));
  connect(this->_cancel_button, SIGNAL(released()),
          this, SLOT(cancel()));
  connect(this->_pause_button, SIGNAL(released()),
          this, SLOT(pause()));

  auto layout = new QHBoxLayout(this);
  // XXX: should but 13, 13, 13, 13 but avatar widget size is strange.
  layout->setContentsMargins(8, 8, 13, 8);
  layout->setSpacing(5);
  this->_layout = layout;
  layout->addWidget(this->_peer_avatar, 0, Qt::AlignLeft);
  {
    auto texts = new QVBoxLayout;
    texts->setContentsMargins(5, 0, 0, 0);

    layout->addLayout(texts, 1);

    texts->addStretch();
    auto user_and_status = new QHBoxLayout;
    texts->addLayout(user_and_status);
    auto username = new QLabel(this);

#define GREY "style=\"text-decoration: none; color: #666666;\""
    if (this->_transaction.sent_to_self())
    {
      auto const& to_your_self = [&] {
        username->setText(QString("<a " GREY ">To</a> yourself"));
      };
      if (this->_transaction.is_sender_device())
      {
        if (this->_transaction.recipient_device())
        {
          username->setText(
            QString("<a " GREY ">To</a> ")
            + this->_transaction.recipient_device().get().name());
        }
        else
        {
          to_your_self();
        }
      }
      else if (this->_transaction.is_recipient_device() ||
               !this->_transaction.has_recipient_device())
      {
        if (this->_transaction.sender_device())
        {
          username->setText(
            QString("<a " GREY ">From</a> ")
            + this->_transaction.sender_device().get().name());
        }
        else
        {
          username->setText(QString("<a " GREY ">From</a> yourself"));
        }
      }
      else
      {
        to_your_self();
      }
    }
    else if (this->_transaction.is_sender())
    {
      username->setText(
        QString("<a style=\"text-decoration: none; color: #666666;\">To</a> ")
        + this->_transaction.peer().fullname());
    }
    else
    {
      username->setText(
        QString("<a style=\"text-decoration: none; color: #666666;\">From</a> ")
        + this->_transaction.peer().fullname());
    }
    {
      view::transaction::peer::style(*username);
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
      this->_filename->setToolTip(this->_transaction.files_tooltip());
      view::transaction::files::style(*this->_filename);
      this->_filename->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
      texts->addWidget(this->_filename);
    }
    texts->addStretch();
  }

  {
    auto time_and_info = new QVBoxLayout;
    time_and_info->setContentsMargins(0, 0, 0, 0);
    time_and_info->setSpacing(10);
    this->_info_area->setLayout(time_and_info);
    time_and_info->addStretch();
    view::transaction::date::style(*this->_mtime);
    time_and_info->addWidget(this->_mtime, 0, Qt::AlignCenter | Qt::AlignRight);
    auto status_and_cancel = new QHBoxLayout;
    time_and_info->addLayout(status_and_cancel);
    time_and_info->addStretch();
    {
      status_and_cancel->setSpacing(5);
      status_and_cancel->addStretch();
      status_and_cancel->addWidget(this->_pause_button, 0, Qt::AlignVCenter | Qt::AlignRight);
      status_and_cancel->addWidget(this->_cancel_button, 0, Qt::AlignVCenter | Qt::AlignRight);
      status_and_cancel->addWidget(this->_status, 0, Qt::AlignVCenter | Qt::AlignRight);
    }
    layout->addWidget(this->_info_area);
  }
  {
    auto infos = new QVBoxLayout;
    this->_accept_reject_area->setLayout(infos);
    infos->setContentsMargins(0, 2, 0, 2);
    infos->addStretch();
    infos->addWidget(this->_accept_button, 0, Qt::AlignCenter);
    infos->addSpacing(2);
    infos->addWidget(this->_reject_button, 0, Qt::AlignCenter);
    infos->addStretch();
    this->_accept_button->setToolTip("Accept");
    this->_reject_button->setToolTip("Reject");

    layout->addWidget(this->_accept_reject_area);
  }

  setSizePolicy(QSizePolicy::Minimum,
                QSizePolicy::Minimum);
  adjustSize();

#ifndef FIST_PRODUCTION_BUILD
  this->setToolTip(this->_transaction.tooltip());
#endif

  this->apply_update();
  this->update();

  this->_update_peer_status();
}

TransactionWidget::~TransactionWidget()
{
  ELLE_DEBUG_SCOPE("%s: destuction", *this);
  if (this->_progress_timer)
  {
    ELLE_DEBUG("destroy timer")
      this->_progress_timer.reset();
  }
}

void
TransactionWidget::_connect_peer()
{
  connect(&this->_transaction.peer(), SIGNAL(avatar_updated()),
          this, SLOT(_on_avatar_updated()));
  connect(&this->_transaction.peer(), SIGNAL(status_updated()),
          this, SLOT(_update_peer_status()));
}

bool
TransactionWidget::eventFilter(QObject *obj, QEvent *event)
{
  static QList<int> events{
    QEvent::Enter, QEvent::Leave, QEvent::MouseButtonPress};
  if (!dynamic_cast<QWidget*>(obj) || !events.contains(event->type()))
    return Super::eventFilter(obj, event);

  if (obj == this->_filename)
  {
    if (this->_transaction.is_recipient_device() &&
        this->_transaction.status() == gap_transaction_finished)
    {
      if (event->type() == QEvent::Enter)
      {
        view::transaction::files::hover_style(*this->_filename);
        this->setCursor(QCursor(Qt::PointingHandCursor));
        this->update();
      }
      else if (event->type() == QEvent::Leave)
      {
        view::transaction::files::style(*this->_filename);
        this->setCursor(QCursor(Qt::ArrowCursor));
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
  this->_peer_avatar->setPicture(this->_transaction.peer().avatar());
  this->repaint();
}

void
TransactionWidget::_update_peer()
{
  this->_connect_peer();
  this->_peer_fullname->setText(this->_transaction.peer().fullname());
  this->_on_avatar_updated();
}

void
TransactionWidget::_update_peer_status()
{
 ELLE_DEBUG("peer is %sconnected",
             this->_transaction.peer().status() ? "" : "dis");
  if (this->_transaction.peer().status())
    this->_peer_status->show();
  else
    this->_peer_status->hide();
}

void
TransactionWidget::apply_update()
{
  ELLE_TRACE_SCOPE("%s: update: %s", *this, this->_transaction.status());
  this->_status->show();
  this->_info_area->show();
  this->_cancel_button->hide();
  this->_pause_button->hide();
  this->_accept_reject_area->hide();

  if (this->_transaction.acceptable())
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
    if (this->_transaction.can_be_canceled())
      this->_cancel_button->show();
    this->_status->show();
    this->_mtime->show();
    this->_info_area->show();
  }
  else
  {
     this->_info_area->hide();
  }

  if (this->_transaction.running() && this->_transaction.concerns_device())
  {
    if (this->_transaction.pause())
    {
      this->_pause_button->set_pixmap(":/conversation/resume");
      this->_pause_button->setToolTip("Resume");
    }
    else
    {
      this->_pause_button->set_pixmap(":/conversation/pause");
      this->_pause_button->setToolTip("Pause");
    }
    this->_pause_button->show();
    this->_status->hide();
  }

  if (this->_transaction.running() && this->_progress_timer == nullptr)
  {
    ELLE_TRACE("run progress timer");
    this->_peer_avatar->setTransactionCount(this->_transaction.files().size());

    this->_progress_timer.reset(new QTimer);
    this->_progress_timer->setInterval(AvatarWidget::progress_update_interval);
    connect(this->_progress_timer.get(), SIGNAL(timeout()),
            this, SLOT(update_progress()));
    this->_progress_timer->start(AvatarWidget::progress_update_interval);

    connect(this, SIGNAL(onProgressChanged(float)),
            this->_peer_avatar, SLOT(setProgress(float)));
  }
  else if (!this->_transaction.running() && this->_progress_timer != nullptr)
  {
    ELLE_TRACE("destroy progress timer");
    setProgress(0);
    this->_progress_timer.reset();
  }

  if (this->_transaction.is_final())
  {
    this->_peer_avatar->setTransactionCount(0);
  }

#ifndef FIST_PRODUCTION_BUILD
  this->setToolTip(this->_transaction.tooltip());
#endif
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
  emit send_metric(UIMetrics_ConversationAccept,
                   std::unordered_map<std::string, std::string>());
}

void
TransactionWidget::reject()
{
  ELLE_TRACE_SCOPE("%s: reject transaction", *this);
  emit transaction_rejected(this->_transaction.id());
  emit send_metric(UIMetrics_ConversationReject,
                   std::unordered_map<std::string, std::string>());
}

void
TransactionWidget::cancel()
{
  ELLE_TRACE_SCOPE("%s: cancel transaction", *this);
  emit transaction_canceled(this->_transaction.id());
  emit send_metric(UIMetrics_ConversationCancel,
                   std::unordered_map<std::string, std::string>());
}

void
TransactionWidget::pause()
{
  ELLE_TRACE_SCOPE("%s: pause transaction", *this);
  emit transaction_paused(this->_transaction.id());
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
                  bool animated = false,
                  QString const& tooltip = ""):
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

  auto tooltip = [] (Model::Status const& status,
                     fist::model::User const& peer) -> StatusUpdater
    {
      switch (status)
      {
        case gap_transaction_new:
          return StatusUpdater(
            QString(), false, "New");
        case gap_transaction_waiting_accept:
          return StatusUpdater(
            QString(":/loading"), true, "Wait for user to accept");
        case gap_transaction_waiting_data:
          return StatusUpdater(
            QString(":/loading"), true, "Waiting for data");
        case gap_transaction_connecting:
          return StatusUpdater(
            QString(":/loading"), true, "Connecting");
        case gap_transaction_transferring:
          return StatusUpdater(QString(), false, "Transferring");
        case gap_transaction_finished:
          return StatusUpdater(
            QString(":/transaction/received"), false, peer.ghost() ? "Ghost uploaded" : "Finished");
        case gap_transaction_cloud_buffered:
          return StatusUpdater(
            QString(":/transaction/sent"), false, "Cloud Buffered");
        case gap_transaction_failed:
          return StatusUpdater(QString(":/conversation/error"), false, "Failed");
        case gap_transaction_canceled:
          return StatusUpdater(QString(":/conversation/canceled"), false, "Canceled");
        case gap_transaction_rejected:
          return StatusUpdater(QString(":/conversation/canceled"), false, "Rejected");
        case gap_transaction_deleted:
          return StatusUpdater(QString(":/conversation/error"), false, "Deleted");
        case gap_transaction_on_other_device:
          return StatusUpdater(QString(":/transaction/sent"), false, "On another device");
        case gap_transaction_paused:
          return StatusUpdater(
            QString(":/transaction/paused"), false, "Paused");
        case gap_transaction_payment_required:
          return StatusUpdater(
            QString(":/conversation/canceled"), false, "Quota exceeded");
      }

      return StatusUpdater(QString(), false, "Something wrong append");
    };

  if (this->_transaction.status() == gap_transaction_finished)
    emit transaction_finished(this->_transaction.id());

  tooltip(this->_transaction.status(), this->_transaction.peer())(
    *this->_status);

  if (this->_transaction.status() == gap_transaction_transferring)
  {
    if (this->_transaction.is_sender())
    {
      this->_status->setPixmap(QPixmap(QString(":/conversation/upload")));
      this->_status->setToolTip(QString("Uploading"));
    }
    else
    {
      this->_status->setPixmap(QPixmap(QString(":/conversation/download")));
      this->_status->setToolTip(QString("Downloading"));
    }
  }
  this->update_mtime();
  if (this->_peer_fullname->text().isEmpty())
    this->_peer_fullname->setText(this->_transaction.peer().fullname());
}

void
TransactionWidget::update_mtime()
{
  ELLE_DUMP_SCOPE("mtime updated");
  auto transaction_mtime = this->_transaction.mtime().toLocalTime();
  this->_mtime->setText(pretty_date(transaction_mtime));
}

void
TransactionWidget::print(std::ostream& stream) const
{
  stream << "TransactionWidget(" << this->_transaction << ")";
}
