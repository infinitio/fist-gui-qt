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
    _accept_button = new QPushButton(QString("Accept"), this);
    connect(_accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    _accept_button->move(this->_avatar->width() + padding, padding);

    infos->addWidget(_accept_button);
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
  return QSize(320, size.height());
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
  // Accept button update.
  if (this->_transaction.status() != gap_transaction_waiting_for_accept &&
      this->_accept_button != nullptr)
  {
    delete this->_accept_button;
    this->_accept_button = nullptr;
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
  emit onAcceptedTransaction(this->_transaction.id());
}
