#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <iostream>

#include <fist-gui-qt/UserWidget.hh>
#include <fist-gui-qt/globals.hh>

UserWidget::UserWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar(), QSize(32, 32))),
  _layout(new QHBoxLayout(this))
{

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
  std::cout << "clicked: " << this->_model.fullname().toStdString() << std::endl;
  emit clicked_signal(this->_model.id());
}

void
UserWidget::_update()
{
  std::cerr << "UserWidget: " << this << " update" << std::endl;
  if (this->_model.new_avatar())
  {
    std::cerr << "new avatar available" << std::endl;
    this->_avatar->set_avatar(this->_model.avatar());
  }
}
