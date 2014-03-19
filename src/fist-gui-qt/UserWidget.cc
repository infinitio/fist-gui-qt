#include <map>

#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include <elle/log.hh>

#include <fist-gui-qt/AvatarWidget.hh>
#include <fist-gui-qt/UserWidget.hh>
#include <fist-gui-qt/UserModel.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.UserWidget");

UserWidget::UserWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent, view::background, true),
  _user(model),
  _peer_avatar(new AvatarWidget(this->_user.avatar())),
  _peer_status(new QLabel),
  _layout(nullptr),
  _notification(new QLabel("notification goes here!")),
  _mtime(new QLabel),
  _status(new QLabel),
  _timer(nullptr)
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);

  this->_peer_status->setPixmap(QPixmap(":/icons/status.png"));

  this->_peer_avatar = new AvatarWidget(this->_user.avatar());

  auto layout = new QHBoxLayout(this);
  // XXX: should but 13, 13, 13, 13 but avatar widget size is strange.
  layout->setContentsMargins(8, 8, 13, 8);
  this->_layout = layout;
  layout->addWidget(this->_peer_avatar, 0, Qt::AlignLeft);

  auto texts = new QVBoxLayout;
  texts->setContentsMargins(5, 0, 5, 0);

  layout->addLayout(texts);

  texts->addStretch();
  auto user_and_status = new QHBoxLayout;
  texts->addLayout(user_and_status);
  auto username = new QLabel(this->_user.fullname());
  {
    view::user::fullname::style(*username);
    username->setMaximumWidth(150);
    username->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    user_and_status->addWidget(username);
  }

  user_and_status->addWidget(this->_peer_status, 0, Qt::AlignLeft);
  user_and_status->addStretch(0);
  texts->addSpacing(4);

  {
    view::user::notification::style(*this->_notification);
    this->_notification->setFixedWidth(170);
    this->_notification->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    texts->addWidget(this->_notification);
  }

  texts->addStretch();
  layout->addStretch();
  auto infos = new QVBoxLayout;
  infos->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(infos);

  layout->addStretch();
  layout->addWidget(this->_status, 0, Qt::AlignCenter | Qt::AlignRight);
  this->_update();

  setSizePolicy(QSizePolicy::Minimum,
                QSizePolicy::Minimum);
  adjustSize();

#if 0
  connect(this->_peer_avatar,
          SIGNAL(onProgressChanged(float)),
          SIGNAL(onProgressChanged(float)));
#endif
  this->update();
}

UserWidget::~UserWidget()
{
  ELLE_TRACE_SCOPE("%s: destruction", *this);
}

/*-----------.
| Properties |
`-----------*/

float
UserWidget::progress() const
{
  return this->_peer_avatar->progress();
}

void
UserWidget::setProgress(float value)
{
  ELLE_DEBUG("%s: update progress to %s", *this, value);
  this->_peer_avatar->setProgress(value);
}

/*-------.
| Layout |
`-------*/

QSize
UserWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(this->widthHint(), size.height());
}

QSize
UserWidget::minimumSizeHint() const
{
  return this->sizeHint();
}

void
UserWidget::trigger()
{
  ELLE_TRACE_SCOPE("%s: clicked", *this);
#if 0
  auto pop = new TransactionWindow(parentWidget());
  parentWidget()->hide();

  pop->show();
  pop->setFocus();
#endif

  emit clicked(this->_user.id());
}

void
UserWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: update", *this);

  if (this->_user.new_avatar())
  {
    ELLE_DEBUG("new avatar");
    this->_peer_avatar->setPicture(this->_user.avatar());
  }

  ELLE_DEBUG("peer is %sconnected",
             this->_user.status() ? "" : "dis");
  if (this->_user.status())
    this->_peer_status->show();
  else
    this->_peer_status->hide();

  this->update_status();
}

void
UserWidget::update_progress()
{
  ELLE_DUMP("%s: update progress", *this);
  // float progress = this->_user.progress();
  emit onProgressChanged(0.0f /*progress*/);
}

/*-------.
| Status |
`-------*/
void
UserWidget::update_status()
{
}

void
UserWidget::print(std::ostream& stream) const
{
  stream << "UserWidget(" << this->_user << ")";
}
