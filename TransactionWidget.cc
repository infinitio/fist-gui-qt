#include <iostream>

#include <QHBoxLayout>
#include <QLabel>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionWindow.hh>

std::map<gap_TransactionStatus, QString> g_statuses =
{
  { gap_transaction_none, "None" },
  { gap_transaction_pending, "Pending" },
  { gap_transaction_copying, "Copying" },
  { gap_transaction_waiting_for_accept, "Waiting for accept" },
  { gap_transaction_accepted, "Accepted" },
  { gap_transaction_preparing, "Preparing" },
  { gap_transaction_running, "In progress" },
  { gap_transaction_cleaning, "Cleaning" },
  { gap_transaction_finished, "Done" },
  { gap_transaction_failed, "Failed" },
  { gap_transaction_canceled, "Canceled" },
  { gap_transaction_rejected, "Rejected" },
};

TransactionWidget::TransactionWidget(TransactionModel const& model):
  _transaction(model),
  _layout(nullptr),
  _accept_button(nullptr),
  _reject_button(nullptr),
  _cancel_button(nullptr),
  _status(new QLabel(g_statuses[this->_transaction.status()])),
  _timer(nullptr)
{
  static int const padding = 5;

  this->_avatar = new AvatarWidget(this->_transaction.avatar());

  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(5, 12, 5, 12);
  this->_layout = layout;
  layout->addWidget(this->_avatar);

  auto texts = new QVBoxLayout;
  texts->setContentsMargins(5, 12, 5, 12);
  layout->addLayout(texts);

  auto username = new QLabel(this->_transaction.peer_fullname());
  auto filename = new QLabel(this->_transaction.files().first());

  {
    username->setFixedWidth(120);
    QFont font;
    font.setBold(true);
    username->setFont(font);
    username->move(this->_avatar->width() + padding, padding);
    texts->addWidget(username);
  }

  {
    filename->setFixedWidth(120);
    QFont font;
    filename->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(150, 150, 150));
    filename->setPalette(palette);
    filename->move(this->_avatar->width() + padding,
                    this->_avatar->height() - filename->height() - padding);
    texts->addWidget(filename);
  }

  auto infos = new QVBoxLayout;
  infos->setContentsMargins(5, 12, 5, 12);
  layout->addLayout(infos);
  infos->addWidget(this->_status);

  if (!this->_transaction.is_sender() &&
      this->_transaction.status() == gap_transaction_waiting_for_accept)
  {
    auto buttons = new QHBoxLayout;
    infos->addLayout(buttons);

    this->_accept_button = new QPushButton(QString("Accept"), this);
    connect(this->_accept_button, SIGNAL(released()), this, SLOT(accept()));

    this->_reject_button = new QPushButton(QString("Reject"), this);
    connect(this->_reject_button, SIGNAL(released()), this, SLOT(reject()));

    buttons->addWidget(this->_accept_button);
    buttons->addWidget(this->_reject_button);
  }
  else
    infos->addWidget(new QLabel());

  if (this->_transaction.status() == gap_transaction_running)
  {
    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()), this, SLOT(update_progress()));
    _timer->start(1000);

    connect(this,
            SIGNAL(onProgressChanged(float)),
            this->_avatar,
            SLOT(setProgress(float)));
  }

  setSizePolicy(QSizePolicy::MinimumExpanding,
                QSizePolicy::Fixed);
  adjustSize();

#if 0
  connect(this->_avatar,
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
  return this->_avatar->progress();
}

void
TransactionWidget::setProgress(float value)
{
  this->_avatar->setProgress(value);
}

/*-------.
| Layout |
`-------*/

QSize
TransactionWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(305, size.height());
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
    std::cerr << "tr widget: " << this->_transaction.peer_fullname().toStdString() << ": update avatar" << std::endl;
    this->_avatar->setPicture(this->_transaction.avatar());
  }

  // Accept button update.
  if (this->_transaction.status() != gap_transaction_waiting_for_accept &&
      this->_accept_button != nullptr)
  {
    delete this->_accept_button;
    this->_accept_button = nullptr;
    delete this->_reject_button;
    this->_reject_button = nullptr;
  }

  if (this->_transaction.status() == gap_transaction_running &&
      this->_timer == nullptr)
  {
    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()), this, SLOT(update_progress()));
    _timer->start(1000);

    connect(this,
            SIGNAL(onProgressChanged(float)),
            this->_avatar,
            SLOT(setProgress(float)));
  }
  else if (this->_transaction.status() != gap_transaction_running &&
           this->_timer != nullptr)
  {
    setProgress(0);
    delete this->_timer;
    this->_timer = nullptr;
  }

  this->_status->setText(g_statuses[this->_transaction.status()]);
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
