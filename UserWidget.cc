#include <QHBoxLayout>
#include <QLabel>

#include <fist-gui-qt/UserWidget.hh>

UserWidget::UserWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent),
  _model(model),
  _layout(new QHBoxLayout(this)),
  _avatar(new AvatarIcon(this->_model.avatar()))
{
  this->_layout->addWidget(this->_avatar);
  this->_layout->addWidget(new QLabel(this->_model.fullname()));

  this->setFixedHeight(this->_avatar->height() + 10);
}

QSize
UserWidget::sizeHint() const
{
  auto size = this->_layout->minimumSize();
  return QSize(this->width(), size.height());
}

void
UserWidget::trigger()
{
  std::cout << "clicked: " << this->_model.fullname().toStdString() << std::endl;
  emit clicked_signal(this->_model.id());
}
