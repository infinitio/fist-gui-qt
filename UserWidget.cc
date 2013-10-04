#include "UserWidget.hh"

UserWidget::UserWidget(QString const& user, QWidget* parent = nullptr):
  ListItem(parent),
  _user(user)
{}

void
UserWidget::trigger()
{
  std::cout << _user.toStdString() << std::endl;
  this->setStyleSheet("background-color:red;");
}

bool
UserWidget::compare_id(const uint32_t uid)
{
  return uid == _uid;
}
