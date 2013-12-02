#include <fist-gui-qt/UserWidget.hh>

UserWidget::UserWidget(QString const& user, uint32_t uid, QWidget* parent):
  ListItem(parent),
  _user(user),
  _uid(uid)
{}

void
UserWidget::trigger()
{
  std::cout << _user.toStdString() << std::endl;
  emit clicked_signal(_uid);
}
