#ifndef FIST_GUI_QT_CUSTOM_EVENTS_CONTACTJOINED_HH
# define FIST_GUI_QT_CUSTOM_EVENTS_CONTACTJOINED_HH

# include <QEvent>

# include <elle/attribute.hh>

namespace fist
{
  class ContactJoined:
    public QEvent
  {
  public:
    ContactJoined(uint32_t id,
                  std::string const& contact);
    virtual
    ~ContactJoined();
  private:
    ELLE_ATTRIBUTE_R(uint32_t, id);
    ELLE_ATTRIBUTE_R(std::string, contact);
  };
}

#endif
