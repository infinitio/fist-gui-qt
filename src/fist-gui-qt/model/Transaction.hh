#ifndef FIST_GUI_QT_TRANSACTIONMODEL_HH
# define FIST_GUI_QT_TRANSACTIONMODEL_HH

# include <boost/logic/tribool.hpp>

# include <QVector>
# include <QString>
# include <QPixmap>
# include <QDateTime>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/model/Model.hh>
# include <surface/gap/gap.hh>


namespace fist
{
  namespace model
  {
    class Transaction
      : public Model
    {
      typedef Model Super;
    public:
      Transaction(fist::State& state,
                    uint32_t id);
      virtual
      ~Transaction() = default;
      Transaction(Transaction const&) = default;

      model::User const&
      peer() const;

      bool
      is_final() const;

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

      QDateTime const&
      mtime() const;

      // XXX: Avatar fetching duplicated from User.
      QPixmap const&
      avatar() const;

      bool
      new_avatar() const;

      void
      avatar_available() const;

    private:
      // Every attributes are marked as mutable in order to allow lazy
      // evaluation.
      ELLE_ATTRIBUTE_RP(boost::logic::tribool, is_sender, mutable);
      ELLE_ATTRIBUTE_rw(gap_TransactionStatus, status);
      ELLE_ATTRIBUTE_P(QString, peer_fullname, mutable);
      ELLE_ATTRIBUTE_RP(uint32_t, peer_id, mutable);
      ELLE_ATTRIBUTE_P(QVector<QString>, files, mutable);
      ELLE_ATTRIBUTE_P(QString, tooltip, mutable);
      ELLE_ATTRIBUTE_P(QDateTime, mtime, mutable);

      // XXX/ Should be there.
      ELLE_ATTRIBUTE_P(QPixmap, avatar, mutable);
      ELLE_ATTRIBUTE_P(bool, default_avatar, mutable);
      ELLE_ATTRIBUTE_P(bool, new_avatar, mutable);

  /*-------------.
  | Orderability |
  `-------------*/
    public:
      bool
      operator ==(Transaction const& t) const;

      bool
      operator <(Transaction const& t) const;

    signals:
      void
      avatar_updated() const;

      void
      status_updated() const;

      void
      peer_status_updated() const;

    private:
  /*----------.
  | Printable |
  `----------*/
      void
      print(std::ostream& stream) const override;

    private:
      Q_OBJECT;

    private:
      // Make State friend in order to allow it to send signals.
      friend fist::State;
    };
  }
}

# include <fist-gui-qt/model/Transaction.hxx>

#endif
