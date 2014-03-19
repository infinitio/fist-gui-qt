#include <map>

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>

#include <elle/log.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionBody.hh>
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

  // If the peer is the recipient.
  /*-------------------------------------------------
  |        _______________________________________  |
  |       |                                       | |
  |       |  Name of the file                     | |
  |       |  .---------------------------------.  | |
  |       |  | Note                            |  | |
  |       |  |                                 |  | |
  |       |  `---------------------------------'  | |
  |       |_______________________________________| |
  |                                                 |
  ---------------------------------------------------*/
  // Else
  /*-------------------------------------------------
  |  ________________________________________       |
  | |                                        |      |
  | |  Name of the file                      |      |
  | |  .---------------------------------.   |      |
  | |  | Note                            |   |      |
  | |  |                                 |   |      |
  | |  `---------------------------------'   |      |
  | |                                        |      |
  | [============                            ]      |
  |                                 progress        |
  ---------------------------------------------------*/

TransactionWidget::TransactionWidget(TransactionModel const& model):
  ListItem(nullptr, view::background, false),
  _transaction(model),
  _layout(nullptr),
  _accept_button(new IconButton(QPixmap(":/buttons/accept.png"))),
  _reject_button(new IconButton(QPixmap(":/buttons/reject.png"))),
  _cancel_button(new IconButton(QPixmap(":/buttons/cancel.png"))),
  _mtime(new QLabel),
  _status(new QLabel),
  _timer(nullptr)
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);

  connect(this->_accept_button, SIGNAL(released()),
          this, SLOT(accept()));
  connect(this->_reject_button, SIGNAL(released()),
          this, SLOT(reject()));
  connect(this->_cancel_button, SIGNAL(released()),
          this, SLOT(cancel()));

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(13, 8, 18, 8);

  view::transaction::date::style(*this->_mtime);
  layout->addWidget(this->_mtime, 0, Qt::AlignCenter);
  layout->addSpacing(6);

  auto bodylayout = new QHBoxLayout();

  auto alignment = (this->_transaction.is_sender()? Qt::AlignRight: Qt::AlignLeft);
  auto body = new TransactionBody(this->_transaction, alignment);
  body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  auto info_layout = new QVBoxLayout();
  info_layout->addStretch();
  info_layout->addWidget(this->_accept_button, 0, Qt::AlignCenter | Qt::AlignLeft);
  info_layout->addWidget(this->_reject_button, 0, Qt::AlignCenter | Qt::AlignLeft);
  info_layout->addWidget(this->_cancel_button, 0, Qt::AlignCenter | Qt::AlignLeft);
  info_layout->addWidget(this->_status, 0, Qt::AlignCenter | Qt::AlignRight);
  info_layout->addStretch();

  if (this->_transaction.is_sender())
  {
    bodylayout->addLayout(info_layout);
    bodylayout->addSpacing(15);
    bodylayout->addWidget(body);
  }
  else
  {
    bodylayout->addWidget(body);
    bodylayout->addSpacing(15);
    bodylayout->addLayout(info_layout);
  }

  body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addLayout(bodylayout);
  this->_layout = layout;

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
  this->_status->hide();
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
TransactionWidget::_show_accept_reject()
{
  this->_accept_button->show();
  this->_reject_button->show();
  this->_cancel_button->hide();
  this->_status->hide();
}

void
TransactionWidget::_hide_accept_reject()
{
  this->_accept_button->hide();
  this->_reject_button->hide();
  this->_cancel_button->show();
}

void
TransactionWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);

  if (this->_transaction.status() == gap_transaction_waiting_for_accept &&
      !this->_transaction.is_sender())
  {
    ELLE_DEBUG("show accept / reject buttons");
    this->_show_accept_reject();
  }
  else
  {
    this->_hide_accept_reject();
  }

  if (g_finals.contains(this->_transaction.status()))
  {
    this->_cancel_button->hide();
    this->_status->show();
  }

  if (this->_transaction.status() == gap_transaction_running &&
      this->_timer == nullptr)
  {
    ELLE_TRACE("run progress timer");

    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()), this, SLOT(update_progress()));
    _timer->start(1000);
  }
  else if (this->_transaction.status() != gap_transaction_running &&
           this->_timer != nullptr)
  {
    delete this->_timer;
    this->_timer = nullptr;
  }

  if (g_finals.contains(this->_transaction.status()))
  {
    // this->_peer_avatar->setTransactionCount(0);
  }

  this->update_status();
}

void
TransactionWidget::update_progress()
{
  ELLE_DUMP("%s: update progress", *this);
  float progress = this->_transaction.progress();
  // emit on_progress_changed(progress);
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
