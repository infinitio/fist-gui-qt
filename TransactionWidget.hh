#ifndef TRANSACTIONWIDGET_HH
# define TRANSACTIONWIDGET_HH

# include <iostream>
# include <QWidget>

# include "fwd.hh"
# include "ListItem.hh"

class TransactionWidget:
  public ListItem
{
public:
  TransactionWidget(uint32_t tid);
  Q_OBJECT;

/*-----------.
| Properties |
`-----------*/

public:
  Q_PROPERTY(float progress
             READ progress
             WRITE setProgress
             NOTIFY onProgressChanged);

public:
  float progress() const;

public Q_SLOTS:
  void setProgress(float value);

Q_SIGNALS:
  void onProgressChanged(float);


/*-------.
| Layout |
`-------*/

public:
  virtual
  QSize
  sizeHint() const override;

  virtual
  QSize
  minimumSizeHint() const override;

  virtual
  void
  trigger();

private:
  uint32_t _tid;
  AvatarWidget* _avatar;
  QLayout* _layout;
};

#endif
