#ifndef TRANSACTIONMODEL_HH
# define TRANSACTIONMODEL_HH

# include <boost/logic/tribool.hpp>

# include <QVector>
# include <QString>
# include <QPixmap>
# include <QDateTime>

# include <elle/Printable.hh>

# include <fist-gui-qt/fwd.hh>
# include <surface/gap/gap.h>

class TransactionModel:
  public elle::Printable
{
public:
  TransactionModel(gap_State* state,
                   uint32_t id);

  TransactionModel(TransactionModel const&) = default;

  uint32_t
  id() const;

  virtual
  bool
  is_sender() const;

  virtual
  uint32_t
  peer_id() const;

  virtual
  float
  progress() const;

  virtual
  QVector<QString> const&
  files() const;

  QString
  tooltip() const;

  virtual
  QString const&
  message() const;

  virtual
  gap_TransactionStatus
  status() const;

  virtual
  QDateTime const&
  mtime() const;

private:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable boost::logic::tribool _is_sender;
  mutable uint32_t _peer_id;
  mutable QVector<QString> _files;
  mutable QString _tooltip;
  mutable QDateTime _mtime;
  mutable QString _message;

public:
  bool
  operator ==(TransactionModel const& t) const;

  bool
  operator <(TransactionModel const& t) const;

  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;
};

# include <fist-gui-qt/TransactionModel.hxx>

#endif
