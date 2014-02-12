#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/UserWidget.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.UserWidget");

UserWidget::UserWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar(), QSize(32, 32))),
  _layout(new QHBoxLayout(this))
{
  ELLE_TRACE_SCOPE("%s: contruction", *this);

  this->_layout->setContentsMargins(10, 10, 10, 10);
  this->_layout->addWidget(this->_avatar);
  this->_layout->addSpacing(10);

  {
    auto texts = new QVBoxLayout();
    {
      auto fullname = new QLabel(this->_model.fullname());
      view::send::user::fullname::style(*fullname);
      texts->addWidget(fullname);
    }
    {
      auto handle = new QLabel(this->_model.fullname());
      view::send::user::handle::style(*handle);
      texts->addWidget(handle);
    }
    this->_layout->addLayout(texts);
  }
  // this->_layout->addWidget(new QLabel(this->_model.fullname()));
  // this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
}

QSize
UserWidget::sizeHint() const
{
  return QSize(this->width(), this->_avatar->height() + 2 * 10);
}

void
UserWidget::trigger()
{
  ELLE_TRACE_SCOPE("%s: clicked", *this);
  emit clicked_signal(this->_model.id());
}

void
UserWidget::_update()
{
  ELLE_TRACE_SCOPE("%s: _update", *this);
  if (this->_model.new_avatar())
  {
    ELLE_DEBUG("avatar available");
    this->_avatar->set_avatar(this->_model.avatar());
  }
}

void
UserWidget::print(std::ostream& stream) const
{
  stream << "UserWidget(" << this->_model << ")";
}
