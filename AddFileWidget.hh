#ifndef ADDFILEITEM_HH
# define ADDFILEITEM_HH

# include <QLabel>
# include <QWidget>

# include <fist-gui-qt/IconButton.hh>

class AddFileWidget:
  public QWidget
{
public:
  AddFileWidget(QWidget* parent = nullptr);

  virtual
  QSize
  sizeHint() const override;

  IconButton*
  attach();

private:
  QLabel* _text;
  IconButton* _attach;
};

#endif
