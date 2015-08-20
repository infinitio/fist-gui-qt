#ifndef FIST_GUI_QT_UTILS_FIREANDFORGET_HH
# define FIST_GUI_QT_UTILS_FIREANDFORGET_HH

# include <functional>

# include <QThread>

namespace fist
{
  class FireAndForget
    : public QThread
  {
  public:
    typedef std::function<void ()> Action;
  public:
    FireAndForget(Action const& action,
                  QObject* parent);

  private:
    void
    run() override;

  private:
    Action _action;
  };
}

#endif
