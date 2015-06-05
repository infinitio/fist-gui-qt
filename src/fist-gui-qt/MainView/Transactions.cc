#include <memory>

#include <Qt>
#include <QVBoxLayout>

#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/notification/Center.hh>
#include <fist-gui-qt/notification/IncomingTransaction.hh>
#include <fist-gui-qt/MainView/Transactions.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/MainView/EmptyState/Transaction.hh>

#include <elle/log.hh>
#include <elle/assert.hh>

ELLE_LOG_COMPONENT("infinit.FIST.MainView.Transactions");

namespace fist
{
  namespace mainview
  {

    Transactions::Transactions(fist::State& state,
                               QWidget* parent)
      :  QWidget(parent)
      , _state(state)
      , _transaction_list(new ListWidget(this, ListWidget::Separator(list::separator::colors), view::background))
      , _widgets()
    {
      this->_transaction_list->setMaxRows(4);
      auto* layout = new QVBoxLayout(this);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setMargin(0);
      layout->addWidget(this->_transaction_list);
      if (this->_state.transactions().get<1>().empty())
      {
        this->_transaction_list->add_widget(
          std::make_shared<empty_state::Transaction>(this));
        return;
      }

      for (model::Transaction const& model: this->_state.transactions().get<1>())
      {
        this->add_transaction(model, true);
      }
    }

    void
    Transactions::add_transaction(uint32_t id)
    {
      ELLE_ASSERT_CONTAINS(this->_state.transactions().get<0>(), id);
      this->add_transaction(*this->_state.transactions().get<0>().find(id));
    }

    void
    Transactions::add_transaction(model::Transaction const& transaction,
                                  bool init)
    {
      if (this->_widgets.empty())
      {
        this->_transaction_list->clearWidgets();
      }

      if (!init &&
          transaction.is_recipient() &&
          !transaction.is_sender_device() &&
          !transaction.has_recipient_device())
      {
        auto* notif = new fist::notification::IncomingTransaction(transaction, this);
        connect(notif, SIGNAL(accept_transaction(uint32_t)),
                &this->_state, SLOT(on_transaction_accepted(uint32_t)));
        fist::notification::center().notify(notif);
      }
      auto widget = std::make_shared<TransactionWidget>(transaction);

      connect(widget.get(), SIGNAL(transaction_accepted(uint32_t)),
              &this->_state, SLOT(on_transaction_accepted(uint32_t)));
      connect(widget.get(), SIGNAL(transaction_rejected(uint32_t)),
              &this->_state, SLOT(on_transaction_rejected(uint32_t)));
      connect(widget.get(), SIGNAL(transaction_canceled(uint32_t)),
              &this->_state, SLOT(on_transaction_canceled(uint32_t)));
      connect(widget.get(), SIGNAL(transaction_paused(uint32_t)),
              &this->_state, SLOT(on_transaction_pause_toggled(uint32_t)));
      connect(widget.get(), SIGNAL(open_file(uint32_t)),
              &this->_state, SLOT(open_file(uint32_t)));
      connect(widget.get(), SIGNAL(send_metric(UIMetricsType, std::unordered_map<std::string, std::string> const&)),
              &this->_state, SLOT(send_metric(UIMetricsType, std::unordered_map<std::string, std::string> const&)));
      this->_widgets[transaction.id()] = widget;
      this->_transaction_list->add_widget(widget,
                                          ListWidget::Position::Top);
      this->update();
      this->updateGeometry();
      emit new_transaction_shown(widget.get());
    }

    void
    Transactions::on_transaction_updated(uint32_t id)
    {
      ELLE_TRACE_SCOPE("%s: update transaction %s", *this, id);

      auto const& transaction = this->_state.transaction(id);
      if (this->_widgets.find(id) == this->_widgets.end())
      {
        ELLE_WARN("%s: update for an non displayed transaction: %s",
                  *this, id);
        this->add_transaction(id);
      }

      if (!transaction.concerns_device())
        return;

      switch (transaction.status())
      {
        case gap_transaction_new:
          if (transaction.is_sender() && transaction.is_sender_device())
          {
            fist::notification::center().notify(
              "Sent!",
              "Your files are on their way");
          }
          break;
        case gap_transaction_rejected:
          if (transaction.is_sender())
          {
            fist::notification::center().notify(
              "Shenanigans!",
              QString("%1 declined your transfer.")
              .arg(transaction.peer().fullname()));
          }
          break;
        case gap_transaction_canceled:
          // Should only be displayed if the user is not the one who cancelled.
          fist::notification::center().notify(
            "Nuts!",
            QString("Your transfer with %1 was cancelled.")
            .arg(transaction.peer().fullname()));
          break;
        case gap_transaction_failed:
          if (transaction.is_sender() && transaction.is_sender_device())
            fist::notification::center().notify(
              "Oh no!",
              QString("%1 couldn't be sent to %2.")
              .arg((transaction.files().size() == 1)
                   ? transaction.files()[0]
                   : QString("your %1 files").arg(transaction.files().size()))
              .arg(transaction.peer().fullname()));
          else if (transaction.is_recipient() && transaction.is_recipient_device())
            fist::notification::center().notify(
              "Oh no!",
              QString("%1 couldn't be received from %2.")
              .arg((transaction.files().size() == 1)
                   ? transaction.files()[0]
                   : QString("%1 files").arg(transaction.files().size()))
              .arg(transaction.peer().fullname()));
          break;
        case gap_transaction_finished:
          if (transaction.is_sender() && transaction.is_sender_device())
            fist::notification::center().notify(
              "Success!",
              QString("%1 received %2.")
              .arg(transaction.peer().fullname())
              .arg((transaction.files().size() == 1)
                   ? transaction.files()[0]
                   : QString("your %1 files").arg(transaction.files().size())));
          else if (transaction.is_recipient() && transaction.is_recipient_device())
            fist::notification::center().notify(
              "Success!",
              QString("%1 received from %2.")
              .arg((transaction.files().size() == 1)
                   ? transaction.files()[0]
                   : QString("%1 files").arg(transaction.files().size()))
              .arg(transaction.peer().fullname()));
          break;
        default:
          break;
      }
    }

    void
    Transactions::print(std::ostream& stream) const
    {
      stream << "Transactions";
    }
  }
}
