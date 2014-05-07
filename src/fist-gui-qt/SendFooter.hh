#ifndef SENDFOOTER_HH
# define SENDFOOTER_HH

# include <elle/attribute.hh>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ShapeButton.hh>

# include <QHBoxLayout>
# include <QSpacerItem>

class SendFooter:
  public Footer
{
public:
  SendFooter();

  virtual
  ~SendFooter() = default;

private:
  ELLE_ATTRIBUTE_R(IconButton*, back);
  ELLE_ATTRIBUTE_R(IconButton*, send);
};


#endif
