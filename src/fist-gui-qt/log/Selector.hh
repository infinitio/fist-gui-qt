#ifndef INFINIT_FIST_LOG_SELECTOR_HH
# define INFINIT_FIST_LOG_SELECTOR_HH

# include <elle/attribute.hh>

namespace fist
{
  namespace log
  {
    class Selector
    {
    public:
      Selector(std::string const& log_file);

      virtual
      ~Selector() {}

      virtual
      std::string const&
      log_file() const;

      virtual
      std::string
      previous_log_file() const;

      virtual
      std::string
      next_log_file() const;

      bool
      remove_current_log() const;

      virtual
      bool
      remove_next_log() const;

      ELLE_ATTRIBUTE(std::string, log_file);
    };
  }
}

#endif
