#include <Qt>
#include <QVBoxLayout>

#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/MainView/Transactions.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TextListItem.hh>

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
        this->_transaction_list->add_widget(new TextListItem("You haven't sent or received any files yet", 70, this));
        return;
      }

      for (model::Transaction const& model: this->_state.transactions().get<1>())
      {
        this->add_transaction(model, true);
      }

      connect(&this->_state, SIGNAL(transaction_updated(uint32_t)),
              this, SLOT(on_transaction_updated(uint32_t)));
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

      if (!transaction.is_sender())
      {
        emit systray_message(
          "Incoming!",
          elle::sprintf(
            "%s wants to send %s to you.",
            transaction.peer_fullname().toStdString(),
            (transaction.files().size() == 1)
              ? transaction.files()[0].toStdString()
              : elle::sprintf("%s files", transaction.files().size())
                                   ).c_str());
      }
      auto widget = new TransactionWidget(transaction);

      connect(widget, SIGNAL(transaction_accepted(uint32_t)),
              &this->_state, SLOT(on_transaction_accepted(uint32_t)));
      connect(widget, SIGNAL(transaction_rejected(uint32_t)),
              &this->_state, SLOT(on_transaction_rejected(uint32_t)));
      connect(widget, SIGNAL(transaction_canceled(uint32_t)),
              &this->_state, SLOT(on_transaction_canceled(uint32_t)));
      connect(widget, SIGNAL(open_file(uint32_t)),
              &this->_state, SLOT(open_file(uint32_t)));
      this->_widgets[transaction.id()] = widget;
      this->_transaction_list->add_widget(widget,
                                          ListWidget::Position::Top);
      this->update();
      this->updateGeometry();
      emit new_transaction_shown(widget);
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

      switch (transaction.status())
      {
        // case gap_transaction_accepted:
        //   if (transaction.is_sender())
        //     emit systray_message(
        //       "Accepted!",
        //       elle::sprintf(
        //         "%s accepted %s.",
        //         transaction.peer_fullname().toStdString(),
        //         (transaction.files().size() == 1)
        //         ? transaction.files()[0].toStdString()
        //         : elle::sprintf("your %s files", transaction.files().size())
        //       ).c_str());
        //   break;
        case gap_transaction_rejected:
          if (transaction.is_sender())
            emit systray_message(
              "Shenanigans!",
              elle::sprintf("%s declined your transfer.",
                            transaction.peer_fullname().toStdString()).c_str(),
              QSystemTrayIcon::Warning);
          break;
        case gap_transaction_canceled:
          // Should only be displayed if the user is not the one who cancelled.
          emit systray_message(
            "Nuts!",
            elle::sprintf("Your transfer with %s was cancelled.",
                          transaction.peer_fullname().toStdString()).c_str());
        case gap_transaction_failed:
          if (transaction.is_sender())
            emit systray_message(
              "Oh no!",
              elle::sprintf(
                "%s couldn't be sent to %s.",
                (transaction.files().size() == 1)
                ? transaction.files()[0].toStdString()
                : elle::sprintf("your %s files", transaction.files().size()),
                transaction.peer_fullname().toStdString()).c_str(),
              QSystemTrayIcon::Warning);
          else
            emit systray_message(
              "Oh no!",
              elle::sprintf(
                "%s couldn't be received from %s.",
                (transaction.files().size() == 1)
                ? transaction.files()[0].toStdString()
                : elle::sprintf("%s files", transaction.files().size()),
                transaction.peer_fullname().toStdString()).c_str(),
              QSystemTrayIcon::Warning);

          break;
        case gap_transaction_finished:
          if (transaction.is_sender())
            emit systray_message(
              "Success!",
              elle::sprintf(
                "%s received %s.",
                transaction.peer_fullname().toStdString(),
                (transaction.files().size() == 1)
                ? transaction.files()[0].toStdString()
                : elle::sprintf("your %s files", transaction.files().size())
                ).c_str());
          else
            emit systray_message(
              "Success!",
              elle::sprintf(
                "%s received from %s.",
                (transaction.files().size() == 1)
                ? transaction.files()[0].toStdString()
                : elle::sprintf("%s files", transaction.files().size()),
                transaction.peer_fullname().toStdString()).c_str());
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
