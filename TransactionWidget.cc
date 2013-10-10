#include <QHBoxLayout>
#include <QLabel>

#include "AvatarWidget.hh"
#include "TransactionWidget.hh"
#include "TransactionWindow.hh"

TransactionWidget::TransactionWidget(gap_State* state, uint32_t tid):
  _tid(tid),
  _state(state),
  _layout(nullptr),
  _accept_button(nullptr)
{
  static int const padding = 5;

  // Retrieve information from gap layer.
  // TODO: Error checking.
  char** file_names = gap_transaction_files(state, tid);
  const char* first_file_name = *file_names;

  uint32_t oid;
  const char* corresp_name;
  if (gap_self_id(state) == gap_transaction_recipient_id(state, tid))
  {
    corresp_name = gap_transaction_sender_fullname(state, tid);
    oid = gap_transaction_sender_id(state, tid);
  }
  else
  {
    corresp_name = gap_transaction_recipient_fullname(state, tid);
    oid = gap_transaction_recipient_id(state, tid);
  }

  this->_avatar = new AvatarWidget(state, oid);

  auto layout = new QHBoxLayout(this);
  this->_layout = layout;
  layout->addWidget(this->_avatar);

  {
    auto texts = new QVBoxLayout;
    texts->setContentsMargins(5, 12, 5, 12);
    layout->addLayout(texts);
    {
      auto username = new QLabel(QString(corresp_name));
      QFont font;
      font.setBold(true);
      username->setFont(font);
      username->move(this->_avatar->width() + padding, padding);
      texts->addWidget(username);
    }
    {
      auto filename = new QLabel(QString(first_file_name));
      QFont font;
      filename->setFont(font);
      QPalette palette;
      palette.setColor(QPalette::WindowText, QColor(150, 150, 150));
      filename->setPalette(palette);
      filename->move(this->_avatar->width() + padding,
                     this->_avatar->height() - filename->height() - padding);
      texts->addWidget(filename);
    }
  }

  if (gap_transaction_recipient_id(_state, _tid) == gap_self_id(_state) and
      gap_transaction_status(state, tid) == gap_transaction_waiting_for_accept)
  {
    _accept_button = new QPushButton(QString("Accept"), this);
    connect(_accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    _accept_button->move(this->_avatar->width() + padding, padding);
  }

  setSizePolicy(QSizePolicy::MinimumExpanding,
                QSizePolicy::Fixed);
  adjustSize();

  connect(this->_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
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
  return QSize(300, size.height());
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


bool
TransactionWidget::compare_id(const uint32_t tid)
{
  return tid == _tid;
}

void
TransactionWidget::update()
{
  gap_TransactionStatus status = gap_transaction_status(_state, _tid);

  if (this->_accept_button != nullptr and
      status != gap_transaction_waiting_for_accept)
  {
    delete this->_accept_button;
    this->_accept_button = nullptr;
  }
}

void
TransactionWidget::accept()
{
  gap_accept_transaction(this->_state, this->_tid);
  std::cout << "accepted " << this << std::endl;
}
