#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QLineEdit>
# include <QPushButton>
# include <QWidget>

# include <surface/gap/gap.h>

# include "SmoothLayout.hh"
# include "UserWidget.hh"
# include "fwd.hh"

class SendPanel:
  public SmoothLayout
{
/*------.
| Types |
`------*/
public:
  typedef SendPanel Self;
  typedef SmoothLayout Super;

/*-------------.
| Construction |
`-------------*/
public:
  SendPanel(gap_State* state);

/*------.
| Files |
`------*/
public:
  void
  addFile(const QString& path);

public slots:
  void send();

/*------.
| Users |
`------*/
public:
  void
  setUsers(QStringList const& users);
  void
  clearUsers();
Q_SIGNALS:
  void onSearchChanged(QString const& search);
private:
  QLineEdit* _search;
  QPushButton* _send;
  ListWidget* _users;
  gap_State* _state;

/*-------.
| Layout |
`-------*/
public:
  virtual
  QSize
  sizeHint() const override;
private:
  QSize _size;

private:
  Q_OBJECT;
};

#endif
