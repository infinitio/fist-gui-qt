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

  gap_UserStatus
  peer_connection_status() const;

  float
  progress() const;

  QVector<QString> const&
  files() const;

  QString
  tooltip() const;

  gap_TransactionStatus
  status() const;

  // XXX: Avatar fetching duplicated from UserModel.
  QPixmap const&
  avatar() const;

  bool
  new_avatar() const;

  void
  avatar_available();

private:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable QString _peer_fullname;
  mutable uint32_t _peer_id;
  mutable QVector<QString> _files;

  // XXX/ Should be there.
  mutable QPixmap _avatar;
  mutable bool _default_avatar;
  mutable bool _new_avatar;
};


#endif