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
