#include <fist-gui-qt/LoginThread.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.FIST.LoginThread");

namespace fist
{
  LoginThread::LoginThread(fist::State& state,
                           QString const& email,
                           QString const& pw,
                           bool is_auto,
                           QObject* parent)
    : QThread(parent)
    , _state(state)
    , _email(email)
    , _pw(pw)
    , _is_auto(is_auto)
  {
    ELLE_TRACE_SCOPE("%s: creation", *this);
  }

  void
  LoginThread::run()
  {
    ELLE_TRACE_SCOPE("%s: run", *this);

    // Cooldown before retrying (in ms).
    auto cooldown = 1000;
    auto max_cooldown = 60000;

    // Will explode if the state is destroyed.
    char* _hash = gap_hash_password(
      this->_state.state(),
      this->_email.toStdString().c_str(),
      this->_pw.toStdString().c_str());
    std::string hash(_hash);
    gap_hash_free(_hash);
    gap_Status res = gap_internal_error;
    while (true)
    {
      res = gap_login(
        this->_state.state(), this->_email.toStdString().c_str(), hash.c_str());
      ELLE_LOG("auto: %s, status: %s", this->_is_auto, res);
      static const QList<gap_Status> error{
        gap_network_error,
          gap_meta_unreachable,
          gap_trophonius_unreachable,
          gap_meta_down_with_message};
      if (this->_is_auto && error.contains(res))
      {
        ELLE_WARN("enable to login: %s (retry)", res);
        QThread::msleep(cooldown);
        cooldown = qMin(cooldown * 2, max_cooldown);
      }
      else
      {
        break;
      }
    }
    emit result_ready(res);
  }
}
