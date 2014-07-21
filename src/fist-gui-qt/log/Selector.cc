#include <QFile>

#include <elle/log.hh>

#include <fist-gui-qt/log/Selector.hh>

ELLE_LOG_COMPONENT("infinit.FIST.log.Selector");

namespace infinit
{
  namespace fist
  {
    namespace log
    {
      Selector::Selector(std::string const& log_file):
        _log_file(log_file)
      {}

      std::string const&
      Selector::log_file() const
      {
        return this->_log_file;
      }

      std::string
      Selector::previous_log_file() const
      {
        return "";
      }

      std::string
      Selector::next_log_file() const
      {
        return "";
      }

      bool
      Selector::remove_current_log() const
      {
        ELLE_TRACE_SCOPE("%s: remove current log: %s", *this, this->_log_file);
        if (this->_log_file.empty())
          return true;
        QFile irrelevant(QString::fromStdString(this->_log_file));
        irrelevant.close();
        if (!irrelevant.exists())
          ELLE_DEBUG("irrelevant log doesn't exist");
        if (!irrelevant.remove())
        {
          ELLE_WARN("impossible to remove log (%s): %s",
                    this->_log_file, irrelevant.errorString());
          if (!irrelevant.rename(QString::fromStdString(this->_log_file + ".old")))
          {
            ELLE_WARN("impossible to rename log (%s): %s",
                      this->_log_file, irrelevant.errorString());
            return false;
          }
        }
        else
          ELLE_DEBUG("irrelevant log %s removed", this->_log_file);
        return true;
      }

      bool
      Selector::remove_next_log() const
      {
        return true;
      }
    }
  }
}
