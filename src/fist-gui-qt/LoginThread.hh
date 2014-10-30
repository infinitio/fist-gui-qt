#ifndef FIST_GUI_QT_LOGINTHREAD_HH
# define FIST_GUI_QT_LOGINTHREAD_HH

# include <QThread>

# include <fist-gui-qt/State.hh>

namespace fist
{
  class LoginThread
    : public QThread
  {
  public:
    LoginThread(fist::State& state,
                QString const& email,
                QString const& pw,
                bool is_auto,
                QObject* parent = nullptr);

  public:
    void
    run() override;

    ELLE_ATTRIBUTE(fist::State&, state);
    ELLE_ATTRIBUTE(QString, email);
    ELLE_ATTRIBUTE(QString, pw);
    ELLE_ATTRIBUTE(bool, is_auto);

  signals:
     void
     result_ready(gap_Status);

  private:
    Q_OBJECT;
  };
}
#endif
