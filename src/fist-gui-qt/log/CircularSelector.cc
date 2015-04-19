#include <QFile>
#include <QDir>

#include <elle/print.hh>
#include <elle/log.hh>

#include <fist-gui-qt/log/CircularSelector.hh>

ELLE_LOG_COMPONENT("infinit.FIST.log.CircularSelector");

namespace fist
{
  namespace log
  {
    namespace
    {
      static
      int
      _available_log_slot(std::string pattern, int rotation_size)
      {
        int available_slot = 0;
        for (int i = 0; i < rotation_size; ++i)
        {
          if (!QFile::exists(
                QDir::toNativeSeparators(
                  QString::fromStdString(elle::sprintf(pattern, i)))))
          {
            available_slot = i;
            break;
          }
        }
        ELLE_DEBUG("available log slot number: %s", available_slot);
        return available_slot;
      }
    }

    CircularSelector::CircularSelector(std::string const& pattern,
                                       int log_rotation_size):
      Selector(elle::sprintf(pattern,
                             _available_log_slot(pattern,
                                                 log_rotation_size))),
      _pattern(pattern),
      _rotation_size(log_rotation_size),
      _available_slot(
        _available_log_slot(this->_pattern, this->_rotation_size)),
      _previous_slot(
        (this->_available_slot + this->_rotation_size - 1)
        % this->_rotation_size),
      _next_slot((this->_available_slot + 1) % this->_rotation_size)
    {
    }

    std::string
    CircularSelector::previous_log_file() const
    {
      return elle::sprintf(this->_pattern, this->_previous_slot);
    }

    std::string
    CircularSelector::next_log_file() const
    {
      return elle::sprintf(this->_pattern, this->_next_slot);
    }

    bool
    CircularSelector::remove_next_log() const
    {
      auto next_slot = QString::fromStdString(this->next_log_file());
      ELLE_DEBUG("remove next slot: %s", next_slot);
      return QFile::remove(next_slot);
    }
  }
}
