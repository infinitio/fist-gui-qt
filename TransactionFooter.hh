#ifndef TRANSACTIONFOOTER_HH
# define TRANSACTIONFOOTER_HH

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>

#include <QHBoxLayout>
#include <QSpacerItem>

class TransactionFooter:
  public Footer
{
public:
  TransactionFooter();

  virtual
  ~TransactionFooter() = default;

  IconButton const&
  send() const;

private:
  IconButton* _send;
};

#endif
