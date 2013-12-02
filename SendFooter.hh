#ifndef SENDFOOTER_HH
# define SENDFOOTER_HH

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>

# include <QHBoxLayout>
# include <QSpacerItem>

class SendFooter:
  public Footer
{
public:
  SendFooter();

  virtual
  ~SendFooter() = default;

Q_SIGNALS:
  void toggle_transaction_view();
  void toggle_file_picker();

private:
  IconButton* _back;
  IconButton* _attach;
  Q_OBJECT
};


#endif
