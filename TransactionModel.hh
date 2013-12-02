#ifndef TRANSACTIONMODEL_HH
# define TRANSACTIONMODEL_HH

# include <QVector>
# include <QString>
# include <QPixmap>

# include <fist-gui-qt/fwd.hh>
# include <surface/gap/gap.h>

class TransactionModel
{
public:
  TransactionModel(gap_State* state,
                   uint32_t id);

  TransactionModel(TransactionModel const&) = default;

  uint32_t
  id() const;

  bool
  is_sender() const;

  uint32_t
  peer_id() const;

  QString const&
  peer_fullname() const;

  float
  progress() const;

  QVector<QString> const&
  files() const;

  gap_TransactionStatus
  status() const;

  QPixmap// const&
  avatar() const;

private:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _peer_fullname;
  mutable uint32_t _peer_id;
  mutable QVector<QString> _files;
};


#endif
