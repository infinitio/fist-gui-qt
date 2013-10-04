#include <QHBoxLayout>
#include <QLabel>

#include "AvatarWidget.hh"
#include "TransactionWidget.hh"
#include "TransactionWindow.hh"

TransactionWidget::TransactionWidget(gap_State* state, uint32_t tid):
  _tid(tid),
  _state(state),
  _avatar(new AvatarWidget),
  _layout(nullptr)
{
  static int const padding = 5;

  // Retrieve information from gap layer.
  // TODO: Error checking.
  char** file_names = gap_transaction_files(state, tid);
  const char* first_file_name = *file_names;

  const char* corresp_name;
  if (gap_self_id(state) == gap_transaction_recipient_id(state, tid))
    corresp_name = gap_transaction_sender_fullname(state, tid);
  else
    corresp_name = gap_transaction_recipient_fullname(state, tid);

  // TODO: retrieve avatar from gap
  auto tmp_avatar = QPixmap(QString("resources/avatar2.png"));
  this->_avatar->setPicture(tmp_avatar);

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

  if (gap_transaction_status(state, tid) == gap_transaction_waiting_for_accept)
  {
    auto button = new QPushButton(QString("Accept"), this);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    button->move(this->_avatar->width() + padding, padding);
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
  auto pop = new TransactionWindow(parentWidget());
  parentWidget()->hide();

  pop->show();
  pop->setFocus();
}


bool
TransactionWidget::compare_id(const uint32_t tid)
{
  return tid == _tid;
}

void
TransactionWidget::update()
{
  std::cout << "UPDATING TRANSACTION WIDGET" << std::endl;
}

void
TransactionWidget::accept()
{
  gap_accept_transaction(this->_state, this->_tid);
  std::cout << "accepted " << this << std::endl;
}
