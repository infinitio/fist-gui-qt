#ifndef FIST_GUI_QT_TRANSACTIONMODEL_HH
# define FIST_GUI_QT_TRANSACTIONMODEL_HH

# include <boost/logic/tribool.hpp>

# include <QVector>
# include <QString>
# include <QPixmap>
# include <QDateTime>

# include <elle/Printable.hh>

# include <fist-gui-qt/fwd.hh>
# include <surface/gap/gap.hh>

class TransactionModel:
  public QObject,
  public elle::Printable
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

  QDateTime const&
  mtime() const;

  // XXX: Avatar fetching duplicated from UserModel.
  QPixmap const&
  avatar() const;

  bool
  new_avatar() const;

  void
  avatar_available() const;

private:
  gap_State* _state;
  uint32_t _id;

  // Every attributes are marked as mutable in order to allow lazy evaluation.
  mutable boost::logic::tribool _is_sender;
  mutable QString _peer_fullname;
  mutable uint32_t _peer_id;
  mutable QVector<QString> _files;
  mutable QString _tooltip;
  mutable QDateTime _mtime;

  // XXX/ Should be there.
  mutable QPixmap _avatar;
  mutable bool _default_avatar;
  mutable bool _new_avatar;

  /*-------------.
  | Orderability |
  `-------------*/
public:
  bool
  operator ==(TransactionModel const& t) const;

  bool
  operator <(TransactionModel const& t) const;

signals:
  void
  avatar_updated() const;

private:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

# include <fist-gui-qt/TransactionModel.hxx>

#endif
