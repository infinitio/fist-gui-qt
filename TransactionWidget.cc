#include <iostream>

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionWindow.hh>
#include <fist-gui-qt/globals.hh>

QVector<gap_TransactionStatus> g_finals =
{
  gap_transaction_cleaning,
  gap_transaction_finished,
  gap_transaction_failed,
  gap_transaction_canceled,
  gap_transaction_rejected,
};

TransactionWidget::TransactionWidget(TransactionModel const& model):
  _transaction(model),
  _peer_avatar(new AvatarWidget(this->_transaction.avatar())),
  _peer_status(new QLabel),
  _layout(nullptr),
  _accept_button(new IconButton(QPixmap(":/buttons/accept.png"))),
  _reject_button(new IconButton(QPixmap(":/buttons/reject.png"))),
  _cancel_button(new IconButton(QPixmap(":/buttons/cancel.png"))),
  _mtime(new QLabel),
  _status(new QLabel),
  _timer(nullptr)
{
  connect(this->_accept_button, SIGNAL(released()),
          this, SLOT(accept()));
  connect(this->_reject_button, SIGNAL(released()),
          this, SLOT(reject()));
  connect(this->_cancel_button, SIGNAL(released()),
          this, SLOT(cancel()));

  this->_peer_avatar = new AvatarWidget(this->_transaction.avatar());

  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  this->_layout = layout;
  layout->addWidget(this->_peer_avatar, 0, Qt::AlignLeft);

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
    // username->setMinimumWidth(0);
    username->setStyleSheet("background-color: pink;");
    username->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    user_and_status->addWidget(username);
  }
  user_and_status->addWidget(this->_peer_status, 0, Qt::AlignLeft);
  user_and_status->addStretch(0);
  texts->addSpacing(4);
  auto filename = new QLabel(this->_transaction.files().first());
  {
    view::transaction::files::style(*filename);
    filename->setFixedWidth(170);
    filename->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    texts->addWidget(filename);
  }

  texts->addStretch();
  layout->addStretch();
  auto infos = new QVBoxLayout;
  infos->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(infos);

  infos->addWidget(this->_accept_button, 0, Qt::AlignCenter | Qt::AlignLeft);
  infos->addWidget(this->_reject_button, 0, Qt::AlignCenter | Qt::AlignLeft);
  infos->addWidget(this->_cancel_button, 0, Qt::AlignCenter | Qt::AlignLeft);

  layout->addStretch();
  layout->addWidget(this->_status, 0, Qt::AlignCenter | Qt::AlignRight);
  this->update();

  // connect(this,
  //         SIGNAL(onProgressChanged(float)),
  //         this->_peer_avatar,
  //         SLOT(setProgress(float)));

  setSizePolicy(QSizePolicy::Minimum,
                QSizePolicy::Minimum);
  adjustSize();

#if 0
  connect(this->_peer_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
#endif
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
  this->_peer_avatar->setProgress(value);
}

/*-------.
| Layout |
`-------*/

QSize
TransactionWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(313, size.height());
}

QSize
TransactionWidget::minimumSizeHint() const
{
  return this->sizeHint();
}

void
TransactionWidget::trigger()
{
#if 0
  auto pop = new TransactionWindow(parentWidget());
  parentWidget()->hide();

  pop->show();
  pop->setFocus();
#endif
}

void
TransactionWidget::update()
{
  if (this->_transaction.new_avatar())
  {
    this->_peer_avatar->setPicture(this->_transaction.avatar());
  }

  if (this->_transaction.status() == gap_transaction_waiting_for_accept &&
      !this->_transaction.is_sender())
  {
    this->_accept_button->show();
    this->_reject_button->show();
  }
  else
  {
    this->_accept_button->hide();
    this->_reject_button->hide();
  }

  if (this->_accept_button->isHidden())
  {
    if (!g_finals.contains(this->_transaction.status()))
    {
      this->_cancel_button->show();
    }
    else
    {
      this->_cancel_button->hide();
    }
    this->_status->show();
  }
  else
  {
    this->_cancel_button->hide();
    this->_status->hide();
  }

  if (this->_transaction.status() == gap_transaction_running &&
      this->_timer == nullptr)
  {
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
  float progress = this->_transaction.progress();
  emit onProgressChanged(progress);
}

void
TransactionWidget::accept()
{
  emit on_transaction_accepted(this->_transaction.id());
}

void
TransactionWidget::reject()
{
  emit on_transaction_rejected(this->_transaction.id());
}

void
TransactionWidget::cancel()
{
  emit on_transaction_canceled(this->_transaction.id());
}

/*-------.
| Status |
`-------*/
void
TransactionWidget::update_status()
{
  switch (this->_transaction.status())
  {
    case gap_transaction_none:
      this->_status->setPixmap(QPixmap());
      this->_status->setToolTip(QString("None"));
      break;
    case gap_transaction_pending:
      this->_status->setPixmap(QPixmap());
      this->_status->setToolTip(QString("Pending"));
      break;
    case gap_transaction_copying:
      this->_status->setPixmap(QPixmap());
      this->_status->setToolTip(QString("Copying"));
      break;
    case gap_transaction_waiting_for_accept:
      this->_status->setMovie(new QMovie(QString(":/icons/loading.gif")));;
      this->_status->movie()->start();
      this->_status->setToolTip(QString("Waiting for peer to be online"));
      break;
    case gap_transaction_accepted:
      this->_status->setPixmap(QPixmap(QString(":/icons/accept.png")));
      this->_status->setToolTip(QString("Accepted"));
      break;
    case gap_transaction_preparing:
      this->_status->setPixmap(QPixmap(QString(":/icons/accept.png")));
      this->_status->setToolTip(QString("Preparing"));
      break;
    case gap_transaction_running:
      if (this->_transaction.is_sender())
        this->_status->setPixmap(QPixmap(QString(":/icons/main-upload.png")));
      else
        this->_status->setPixmap(QPixmap(QString(":/icons/main-download.png")));
      this->_status->setToolTip(QString("Running"));
      break;
    case gap_transaction_cleaning:
      this->_status->setPixmap(QPixmap());
      this->_status->setToolTip(QString("Cleaning"));
      break;
    case gap_transaction_finished:
      this->_status->setPixmap(QPixmap(QString(":/icons/accept.png")));
      this->_status->setToolTip(QString("Finished"));
      break;
    case gap_transaction_failed:
      this->_status->setPixmap(QPixmap(QString(":/icons/error.png")));
      this->_status->setToolTip(QString("Failed"));
      break;
    case gap_transaction_canceled:
      this->_status->setPixmap(QPixmap(QString(":/icons/reject.png")));
      this->_status->setToolTip(QString("Canceled"));
      break;
    case gap_transaction_rejected:
      this->_status->setPixmap(QPixmap(QString(":/icons/reject.png")));
      this->_status->setToolTip(QString("Rejected"));
      break;
    default:
      std::cerr << "lul" << std::endl;
  }
}
