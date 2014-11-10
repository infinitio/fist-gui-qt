#ifndef FIST_GUI_QT_FWD_HH
# define FIST_GUI_QT_FWD_HH

// Windows.
class InfinitDock;
class LoadingDialog;
class Updater;

// Dock Elements.
class AvatarWidget;
class FileItem;
class IconButton;
class Footer;
class ListWidget;
class TextListItem;
class Panel;
class RoundShadowWidget;
class SearchField;
class TransactionList;
class MainPanel;
class TransactionWidget;
class SearchResultWidget;
class HorizontalSeparator;

namespace fist
{
  class State;

  namespace login
  {
    class Window;
    class Footer;
  }
  namespace model
  {
    class User;
    class Transaction;
    // class Link;
  }

  namespace sendview
  {
    class Panel;
    class Footer;
    class Files;
    class Users;
  }

  namespace onboarding
  {
    class Onboarder;
  }
}
#endif
