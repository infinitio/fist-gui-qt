#ifndef FIST_GUI_QT_NOTIFICATION_INCOMINGTRANSACTION_HH
# define FIST_GUI_QT_NOTIFICATION_INCOMINGTRANSACTION_HH

# include <fist-gui-qt/notification/Notification.hh>

# include <fist-gui-qt/fwd.hh>

namespace fist
{
  namespace notification
  {
    class IncomingTransaction
      : public INotification
    {
      typedef INotification Super;
    public:
      IncomingTransaction(model::Transaction const& transaction,
                          QWidget* parent = nullptr);

      ELLE_ATTRIBUTE_R(model::Transaction const&, transaction);

    private slots:
      void
      _accept();

    signals:
      void
      accept_transaction(uint32_t id);

    private:
      void
      paintEvent(QPaintEvent*) override;

    private:
      QPixmap _avatar;
    private:
      Q_OBJECT;
    };
  }
}

#endif
