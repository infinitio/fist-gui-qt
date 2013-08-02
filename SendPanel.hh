#ifndef SENDPANEL_HH
# define SENDPANEL_HH

# include <QLineEdit>
# include <QWidget>

# include "SmoothLayout.hh"
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
  SendPanel();

/*------.
| Files |
`------*/
public:
  void
  addFile(const QString& path);

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
  ListWidget* _users;

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
