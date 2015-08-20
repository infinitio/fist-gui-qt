#include <fist-gui-qt/utils/FireAndForget.hh>

namespace fist
{
  FireAndForget::FireAndForget(Action const& action,
                               QObject* parent)
    : QThread(parent)
    , _action(action)
  {
    connect(this, SIGNAL(finished()), SLOT(deleteLater()));
    this->start();
  }

  void
  FireAndForget::run()
  {
    this->_action();
  }
}
