#ifndef USERPANEL_HH
# define USERPANEL_HH

# include <set>
# include <unordered_map>
# include <algorithm>

# include <QScrollArea>
# include <QFrame>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/gap.h>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/UserFooter.hh>
# include <fist-gui-qt/UserWidget.hh>
# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/UserModel.hh>
# include <fist-gui-qt/fwd.hh>

class UserPanel:
  public Panel
{
public:
  UserPanel(gap_State* state,
            QWidget* parent = nullptr);

public Q_SLOTS:
  UserWidget*
  add_user(gap_State* state,
           uint32_t tid,
           bool init = false);

  void
  avatar_available(uint32_t uid);

  void
  user_status_changed(uint32_t uid,
                      gap_UserStatus status);

  void
  setFocus();

Q_SIGNALS:
  void
  user_clicked(uint32_t);

private:
  ListWidget* _list;
  gap_State* _state;

public:
  UserFooter*
  footer();

  typedef std::unordered_map<uint32_t, UserModel> Users;
  ELLE_ATTRIBUTE_R(Users, users);

private:
  Q_OBJECT;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
