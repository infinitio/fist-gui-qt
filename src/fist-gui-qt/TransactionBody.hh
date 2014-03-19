#ifndef TRANSACTIONBODY_HH
# define TRANSACTIONBODY_HH

# include <QColor>
# include <QLabel>
# include <QGridLayout>
# include <QWidget>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <fist-gui-qt/fwd.hh>

class TransactionBody:
  public QWidget,
  public elle::Printable
{
/*------------.
| Contruction |
`------------*/
public:
  TransactionBody(TransactionModel const& transaction,
                  Qt::AlignmentFlag alignment,
                  QWidget* parent = nullptr);

public Q_SLOTS:
  void
  update_progress();

protected:
  void
  paintEvent(QPaintEvent* event) override;

  QSize
  sizeHint() const override;

private:
  ELLE_ATTRIBUTE(TransactionModel const&, transaction);
  ELLE_ATTRIBUTE(QGridLayout*, layout);
  ELLE_ATTRIBUTE(Qt::AlignmentFlag, alignment);
  ELLE_ATTRIBUTE(QColor, background);
  ELLE_ATTRIBUTE(QLabel*, text_progress);

/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT
};

#endif
