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
# include <surface/gap/PeerTransaction.hh>

namespace fist
{
  namespace model
  {
    class Transaction
      : public Model
    {
    private:
      typedef Model Super;
    public:
      typedef gap_TransactionStatus Status;
    public:
      Transaction(fist::State& state,
                  uint32_t id);
      Transaction(fist::State& state,
                  surface::gap::PeerTransaction const& transaction);
      virtual
      ~Transaction() = default;
      Transaction(Transaction const&) = default;

      bool
      is_sender() const;

      bool
      is_recipient() const;

      bool
      is_sender_device() const;

      bool
      is_recipient_device() const;

      bool
      has_recipient_device() const;

      bool
      concerns_device() const;

      model::User const&
      peer() const;

      void
      on_peer_changed() const;

      QString
      files_tooltip() const;

      bool
      acceptable() const;

      bool
      running() const;

      bool
      is_final() const;

      float
      progress() const;

      QVector<QString> const&
      files() const;

      gap_TransactionStatus
      status() const;

      void
      status(gap_TransactionStatus status);

      void
      mtime(double mtime);
    private:
      ELLE_ATTRIBUTE_R(surface::gap::PeerTransaction, transaction);
    public:
      bool
      transaction(surface::gap::PeerTransaction const& tr,
                  bool notify = true);
    private:
      // Cache the files as a QVector<Qstring>.
      ELLE_ATTRIBUTE_P(QVector<QString>, files, mutable);
      // Cache the mtime.
      ELLE_ATTRIBUTE_R(QDateTime, mtime);
      ELLE_ATTRIBUTE_RX(bool, pause);
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
      status_updated();

      void
      peer_changed() const;


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

#ifndef FIST_PRODUCTION_BUILD
      ELLE_ATTRIBUTE_RP(QString, tooltip, mutable);
#endif
    };
  }
}

# include <fist-gui-qt/model/Transaction.hxx>

#endif
