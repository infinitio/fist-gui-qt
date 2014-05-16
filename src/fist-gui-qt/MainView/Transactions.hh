#ifndef FIST_GUI_QT_MAIN_VIEW_TRANSACTIONS_HH
# define FIST_GUI_QT_MAIN_VIEW_TRANSACTIONS_HH

# include <QSystemTrayIcon>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/TransactionWidget.hh>
# include <fist-gui-qt/model/Transaction.hh>
# include <fist-gui-qt/State.hh>

namespace fist
{
  namespace mainview
  {
    class Transactions
      : public QWidget
      , public elle::Printable
    {
      typedef QWidget Super;
    public:
      Transactions(fist::State& state,
                   QWidget* parent);

    public slots:
      void
      add_transaction(model::Transaction const& model,
                      bool init = false);

      void
      add_transaction(uint32_t id);

      void
      on_transaction_updated(uint32_t tid);

    signals:
      void
      new_transaction(uint32_t id);

      void
      new_transaction_shown(TransactionWidget* widget);

      void
      systray_message(QString const& title,
                      QString const& body,
                      QSystemTrayIcon::MessageIcon = QSystemTrayIcon::Information);

    private:
      typedef std::unordered_map<uint32_t, TransactionWidget*> Widgets;
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(ListWidget*, transaction_list);
      ELLE_ATTRIBUTE(Widgets, widgets);
    private:
      Q_OBJECT;

      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;
    };
  }
}
#endif
