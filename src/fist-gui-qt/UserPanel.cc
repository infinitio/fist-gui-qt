#include <elle/log.hh>

#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/UserPanel.hh>
#include <fist-gui-qt/UserWidget.hh>
#include <fist-gui-qt/UserFooter.hh>

#define MAX_TRANSAS 15

static UserPanel* g_panel = nullptr;
static gap_State* g_state = nullptr;

ELLE_LOG_COMPONENT("infinit.FIST.UserPanel");

UserPanel::UserPanel(gap_State* state,
                     QWidget* parent):
  Panel(new UserFooter, parent),
  _list(new ListWidget(this)),
  _state(state)
{
  ELLE_TRACE_SCOPE("%s: construction", *this);
  this->footer()->setParent(this);

  // Register gap callback.
  g_panel = this;

  uint32_t* users = gap_swaggers(state);

  for (uint32_t i = 0;
       users[i] != gap_null();
       ++i)
  {
    ELLE_DEBUG("add swagger %s", users[i])
    {
      auto widget = this->add_user(state, users[i], true);
      connect(widget, SIGNAL(clicked(uint32_t)),
              this, SIGNAL(user_clicked(uint32_t)));
    }
  }

  gap_swaggers_free(users);
}

UserWidget*
UserPanel::add_user(gap_State* state,
                    uint32_t tid,
                    bool init)
{
  if (this->_users.find(tid) == this->_users.end())
    this->_users.emplace(tid, UserModel(state, tid));

  auto widget = new UserWidget(this->_users.at(tid));
  this->_list->add_widget(widget,
                          init ?
                            ListWidget::Position::Bottom :
                            ListWidget::Position::Top);
  return widget;
}

void
UserPanel::setFocus()
{
  this->_list->setFocus();
}

void
UserPanel::avatar_available(uint32_t uid)
{
  bool update_list = false;
  for (auto& tr: this->_users)
    if (tr.second.id() == uid)
      update_list = true;

  if (update_list)
    this->_list->update();
}

void
UserPanel::user_status_changed(uint32_t /* uid */,
                                      gap_UserStatus /* status */)
{
  this->_list->update();
}

/*-------.
| Footer |
`-------*/
UserFooter*
UserPanel::footer()
{
  return static_cast<UserFooter*>(this->_footer);
}

void
UserPanel::print(std::ostream& stream) const
{
  stream << "UserPanel";
}
