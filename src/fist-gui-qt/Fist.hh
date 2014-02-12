#ifndef FIST_HH
# define FIST_HH

# include <memory>
# include <vector>

# include <QApplication>
# include <QObject>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/gap.h>

class Fist:
  public QObject,
  public elle::Printable
{
/*------------.
| Contruction |
`------------*/
public:
  Fist(int argc, char** argv);
  Fist(QApplication&& application);
  ~Fist();

/*---------------.
| Initialization |
`---------------*/
private:
  void
  _initialize_application();

/*------------------.
| Desinitialization |
`------------------*/
public slots:
  // This slot will be linked to application 'aboutToQuit' signal in order to do
  // 'some last-second cleanup'.
  void
  about_to_quit();

/*----.
| Run |
`----*/
public:
  int
  operator ()();

private:
  ELLE_ATTRIBUTE(std::unique_ptr<QApplication>, application);
  ELLE_ATTRIBUTE(gap_State*, state);
private:
  Q_OBJECT;

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;
};

#endif
