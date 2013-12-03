#include <QHBoxLayout>
#include <QLabel>

#include <fist-gui-qt/UserWidget.hh>

UserWidget::UserWidget(UserModel const& model,
                       QWidget* parent):
  ListItem(parent),
  _model(model),
  _avatar(new AvatarIcon(this->_model.avatar()))
{
  auto layout = new QHBoxLayout(this);
  layout->addWidget(this->_avatar);
  layout->addWidget(new QLabel(this->_model.fullname()));
}

void
UserWidget::trigger()
{
  std::cout << "clicked: " << this->_model.fullname().toStdString() << std::endl;
  emit clicked_signal(this->_model.id());
}
