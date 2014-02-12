#ifndef SENDFOOTER_HH
# define SENDFOOTER_HH

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

public:
  IconButton*
  back() const;

  IconButton*
  send() const;

private:
  IconButton* _back;
  IconButton* _send;
};


#endif
