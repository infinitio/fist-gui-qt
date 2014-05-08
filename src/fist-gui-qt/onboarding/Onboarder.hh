#ifndef FIST_GUI_QT_ONBOARDING_ONBOARDER_HH
# define FIST_GUI_QT_ONBOARDING_ONBOARDER_HH

# include <memory>
# include <unordered_map>

# include <elle/attribute.hh>

# include <fist-gui-qt/InfinitDock.hh>
# include <fist-gui-qt/gui/Tooltip.hh>

namespace fist
{
  namespace onboarding
  {
    // Onboarder manages tooltips to drive the user into the gui.
    // To see the exact process, take a look at resources.
    // The public api allows you to run:
    // - a reception process.
    // - a sending process.
    class Onboarder:
      public QObject
    {
    public:
      // Create an onboarder on the dock.
      // Friendship gives it access to everything.
      Onboarder(InfinitDock* parent);
      virtual
      ~Onboarder();

    public slots:
      // Start the process of receiving a file.
      // The transaction in completely faked by the core of the application.
      // The file passed as argument will be copied into the user downloads
      // folder.
      void
      receive_file(QString const& file);

      // Start the process of sending a file.
      // This method just add tooltips in order to guide the user through the
      // sending process.
      // The file will be sent.
      void
      send_file();

    private slots:
      //
      void
      _on_new_transaction(uint32_t id);

      void
      _on_transaction_accepted(uint32_t id);

      void
      _on_transaction_rejected(uint32_t id);

      void
      _on_transaction_canceled(uint32_t id);

      void
      _on_transaction_finished(uint32_t id);

      void
      _on_transaction_widget_shown(TransactionWidget* widget);

      void
      _on_send_panel_visible();

      void
      _choose_peer();

      void
      _on_peer_chosen();

      void
      _on_file_added();

      void
      _on_file_added_before_peer();

      void
      _on_transaction_ready();

      // Replace the current tooltip by a new one with anchored to the given
      // widget.
      void
      _set_tooltip(QWidget* widget,
                   QString const& text,
                   Qt::AlignmentFlag alignment = Qt::AlignLeft,
                   int duration = 20000);

      void
      _send_onboarding_done();

    signals:
      // Emit if the reception is completed (successfully or not).
      void
      reception_completed();

      // Emit if the sending process is completed (successfully or not).
      void
      sending_completed();

    private:
      ELLE_ATTRIBUTE(InfinitDock*, dock);
      // In order to make the process simpler for debugging, we store the widget
      // linked to the onboarding transaction.
      typedef std::unordered_map<uint32_t, TransactionWidget*> Transactions;
      ELLE_ATTRIBUTE(Transactions, transactions);
      // This implementation allows only one tooltip to be active at the same
      // time.
      ELLE_ATTRIBUTE_X(std::unique_ptr<fist::gui::Tooltip>, tooltip);

    private:
      Q_OBJECT;
    };
  }
}


#endif
