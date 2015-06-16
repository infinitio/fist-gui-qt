#ifndef INFINIT_FIST_LOG_CIRCULARSELECTOR_HH
# define INFINIT_FIST_LOG_CIRCULARSELECTOR_HH

# include <elle/attribute.hh>

# include <fist-gui-qt/log/Selector.hh>

namespace fist
{
  namespace log
  {
    class CircularSelector:
      public Selector
    {
    public:
      CircularSelector(std::string const& pattern,
                       int log_rotation_size = 4);

      std::string
      previous_log_file() const override;

      std::string
      next_log_file() const override;

      bool
      remove_next_log() const override;

      ELLE_ATTRIBUTE(std::string, pattern);
      ELLE_ATTRIBUTE(int, rotation_size);
      ELLE_ATTRIBUTE(int, available_slot);
      ELLE_ATTRIBUTE(int, previous_slot);
      ELLE_ATTRIBUTE(int, next_slot);
    };
  }
}

#endif
