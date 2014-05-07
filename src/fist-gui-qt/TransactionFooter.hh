#ifndef FIST_GUI_QT_TRANSACTIONFOOTER_HH
# define FIST_GUI_QT_TRANSACTIONFOOTER_HH

# include <QHBoxLayout>
# include <QSpacerItem>

# include <elle/attribute.hh>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>

class TransactionFooter:
  public Footer
{
public:
  TransactionFooter();

  virtual
  ~TransactionFooter() = default;

private:
  ELLE_ATTRIBUTE_R(IconButton*, menu);
  ELLE_ATTRIBUTE_R(IconButton*, send);
};

#endif
