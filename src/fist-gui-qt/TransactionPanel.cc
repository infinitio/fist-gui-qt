#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TransactionModel.hh>
#include <fist-gui-qt/UserModel.hh>
#include <fist-gui-qt/UserWidget.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionPanel");

#define MAX_TRANSAS 15

static TransactionPanel* g_panel = nullptr;
static gap_State* g_state = nullptr;

TransactionPanel::TransactionPanel(gap_State* state, QWidget* parent):
  Panel(new TransactionFooter, parent),
  _state(state),
  _peer(nullptr),
  _peer_widget(nullptr),
  _list(nullptr)
{
  this->footer()->setParent(this);
}

TransactionPanel::~TransactionPanel()
{
  this->_peer_widget.reset();
  this->_list.reset();
}

TransactionWidget*
TransactionPanel::add_transaction(TransactionModel const& model,
                                  bool init)
{
  ELLE_TRACE_SCOPE("%s: add transaction %s", *this, model);
  ELLE_ASSERT(this->_list != nullptr);

  auto widget = new TransactionWidget(model);

  connect(widget, SIGNAL(on_transaction_accepted(uint32_t)),
          this, SLOT(_on_transaction_accepted(uint32_t)));
  connect(widget, SIGNAL(on_transaction_rejected(uint32_t)),
          this, SLOT(_on_transaction_rejected(uint32_t)));
  connect(widget, SIGNAL(on_transaction_canceled(uint32_t)),
          this, SLOT(_on_transaction_canceled(uint32_t)));

  this->_list->add_widget(widget,
                          init ?
                            ListWidget::Position::Bottom :
                            ListWidget::Position::Top);

  return widget;
}

void
TransactionPanel::setFocus()
{
  this->_list->setFocus();
}

void
TransactionPanel::_on_transaction_accepted(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: accept transaction %s", *this, tid);
  gap_accept_transaction(this->_state, tid);
}

void
TransactionPanel::_on_transaction_rejected(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: reject transaction %s", *this, tid);
  gap_reject_transaction(this->_state, tid);
}

void
TransactionPanel::_on_transaction_canceled(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: cancel transaction %s", *this, tid);
  gap_cancel_transaction(this->_state, tid);
}

void
TransactionPanel::transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  ELLE_TRACE_SCOPE("transaction %s updated with status %s", id, status);
  g_panel->_transaction_cb(id, status);
}

void
TransactionPanel::_transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  // if (gap_transaction_status(this->_state, id) <
  //     gap_transaction_waiting_for_accept)
  //   return; // Ignore early status because data may not be fully merged.

  // if (this->_transactions.find(id) == this->_transactions.end())
  // {
  //   this->_transactions.emplace(id, TransactionModel(this->_state, id));
  //   ELLE_DEBUG("new transaction");
  //   this->add_transaction(this->_state, id);
  // }
  // else
  // {
  //   ELLE_DEBUG("update transaction");
  //   this->updateTransaction(this->_state, id);
  // }
}


void
TransactionPanel::updateTransaction(gap_State* /* state */, uint32_t id)
{
  ELLE_TRACE_SCOPE("%s: update transaction %s", *this, id);

//   auto const& transaction = this->_transactions.at(id);

//   switch (transaction.status())
//   {
//     case gap_transaction_accepted:
//       if (transaction.is_sender())
//         emit systray_message(
//           "Accepted!",
//           elle::sprintf(
//             "%s accepted %s.",
//             transaction.peer_fullname().toStdString(),
//             (transaction.files().size() == 1)
//             ? transaction.files()[0].toStdString()
//             : elle::sprintf("your %s files", transaction.files().size())
//             ).c_str());
//       break;
//     case gap_transaction_rejected:
//       if (transaction.is_sender())
//         emit systray_message(
//           "Shenanigans!",
//           elle::sprintf("%s declined your transfer.",
//                         transaction.peer_fullname().toStdString()).c_str(),
//           QSystemTrayIcon::Warning);
//       break;
//     case gap_transaction_canceled:
//       Should only be displayed if the user is not the one who cancelled.
//                                         emit systray_message(
//                                           "Nuts!",
//                                           elle::sprintf("Your transfer with %s was cancelled.",
//                                                         transaction.peer_fullname().toStdString()).c_str());
//     case gap_transaction_failed:
//       if (transaction.is_sender())
//         emit systray_message(
//           "Oh no!",
//           elle::sprintf(
//             "%s couldn't be sent to %s.",
//             (transaction.files().size() == 1)
//             ? transaction.files()[0].toStdString()
//             : elle::sprintf("your %s files", transaction.files().size()),
//             transaction.peer_fullname().toStdString()).c_str(),
//           QSystemTrayIcon::Warning);
//       else
//         emit systray_message(
//           "Oh no!",
//           elle::sprintf(
//             "%s couldn't be received from %s.",
//             (transaction.files().size() == 1)
//             ? transaction.files()[0].toStdString()
//             : elle::sprintf("%s files", transaction.files().size()),
//             transaction.peer_fullname().toStdString()).c_str(),
//           QSystemTrayIcon::Warning);

//       break;
//     case gap_transaction_finished:
//       if (transaction.is_sender())
//         emit systray_message(
//           "Success!",
//           elle::sprintf(
//             "%s received %s.",
//             transaction.peer_fullname().toStdString(),
//             (transaction.files().size() == 1)
//             ? transaction.files()[0].toStdString()
//             : elle::sprintf("your %s files", transaction.files().size())
//             ).c_str());
//       else
//         emit systray_message(
//           "Success!",
//           elle::sprintf(
//             "%s received from %s.",
//             (transaction.files().size() == 1)
//             ? transaction.files()[0].toStdString()
//             : elle::sprintf("%s files", transaction.files().size()),
//             transaction.peer_fullname().toStdString()).c_str());
//       break;
//     default:
//       break;
//   }
//   for (auto widget: this->_list->widgets())
//     widget->_update();
}

/*------------.
| Show & Hide |
`------------*/

void
TransactionPanel::peer(UserModel const* peer)
{
  ELLE_TRACE_SCOPE("%s: set peer", *this);

  this->_peer = peer;

  if (peer != nullptr)
  {
    ELLE_DEBUG("peer: %s", *peer);
    // ELLE_ASSERT(this->_peer_widget == nullptr);
    // ELLE_ASSERT(this->_list == nullptr);

    this->_peer_widget.reset(new UserWidget(*this->_peer, this));
    this->_list.reset(new ListWidget(this));

    for (auto const& transaction: this->_peer->transactions())
      this->add_transaction(transaction);

    this->footer()->setParent(nullptr);
    this->footer()->setParent(this);
  }
  else
  {
    ELLE_DEBUG("reset");

    this->_list.reset();
    this->_peer_widget.reset();
  }
}

void
TransactionPanel::on_show()
{
  ELLE_TRACE_SCOPE("%s: show", *this);
  ELLE_ASSERT(this->_peer != nullptr);
  this->update();
}

void
TransactionPanel::on_hide()
{
  ELLE_TRACE_SCOPE("%s: hide", *this);
  this->update();
}

/*-------.
| Footer |
`-------*/
TransactionFooter*
TransactionPanel::footer()
{
  return static_cast<TransactionFooter*>(this->_footer);
}

void
TransactionPanel::print(std::ostream& stream) const
{
  stream << "TransactionPanel";
}
