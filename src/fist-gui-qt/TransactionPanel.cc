#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TextListItem.hh>

#include <elle/log.hh>
#include <elle/assert.hh>

ELLE_LOG_COMPONENT("infinit.FIST.MainPanel");

#define MAX_TRANSAS 15

static MainPanel* g_panel = nullptr;
static gap_State* g_state = nullptr;

MainPanel::MainPanel(gap_State* state, QWidget* parent):
  Panel(new TransactionFooter, parent),
  _tabs(new Tabber(this)),
  _transaction_list(new ListWidget(this)),
  // _link_list(new ListWidget(this)),
  _state(state)
{
  this->footer()->setParent(this);



  // Register gap callback.
  g_panel = this;
  gap_transaction_callback(state, MainPanel::transaction_cb);

  //TODO: kill this abomination with a gap_transactions already sorted.
  uint32_t* trs = gap_transactions(state);

  g_state = state;
  struct trs_compare
  {
    std::unordered_map<uint32_t, double> mmap;

    bool
    operator() (const uint32_t& lhs, const uint32_t& rhs)
    {
      if (mmap.find(lhs) == mmap.end())
        mmap.insert({lhs, gap_transaction_mtime(g_state, lhs)});
      if (mmap.find(rhs) == mmap.end())
        mmap.insert({rhs, gap_transaction_mtime(g_state, rhs)});

      return mmap[lhs] > mmap[rhs];
    }
  };

  std::set<uint32_t, trs_compare> transactions_list;

  for (uint32_t v = 0; trs[v] != 0; v += 1)
    transactions_list.insert(trs[v]);
  gap_transactions_free(trs);

  if (transactions_list.empty())
  {
    this->_transaction_list->add_widget(new TextListItem("You haven't sent or received any files yet", 70, this));
    return;
  }

  auto iter = transactions_list.begin();
  for (uint32_t i = 0;
       iter != transactions_list.end() && i < MAX_TRANSAS;
       ++i, ++iter)
  {

    this->add_transaction(state, *iter, true);
  }

  this->_tabs->add_tab("Transactions", this->_transaction_list);
  this->_tabs->add_tab("Links");


  this->_layout();
}

TransactionWidget*
MainPanel::add_transaction(gap_State* state,
                                  uint32_t tid,
                                  bool init)
{
  if (this->_transactions.size() == 0)
  {
    this->_transaction_list->clearWidgets();
  }

  if (this->_transactions.find(tid) == this->_transactions.end())
    this->_transactions[tid].reset(new TransactionModel(state, tid));

  auto const& transaction = *this->_transactions.at(tid);
  emit new_transaction(tid);

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
          this, SLOT(_on_transaction_accepted(uint32_t)));
  connect(widget, SIGNAL(transaction_rejected(uint32_t)),
          this, SLOT(_on_transaction_rejected(uint32_t)));
  connect(widget, SIGNAL(transaction_canceled(uint32_t)),
          this, SLOT(_on_transaction_canceled(uint32_t)));

  this->_transaction_list->add_widget(widget,
                          init ?
                            ListWidget::Position::Bottom :
                            ListWidget::Position::Top);
  this->update();
  this->updateGeometry();
  emit new_transaction_shown(widget);
  return widget;
}

void
MainPanel::setFocus()
{
  // this->_transaction_list->setFocus();
}

void
MainPanel::avatar_available(uint32_t uid)
{
  ELLE_TRACE_SCOPE("%s: avatar available for user %s", *this, uid);
  // XXX: Ugly, but no better way for the moment.
  for (auto& tr: this->_transactions)
  {
    if (tr.second->peer_id() == uid)
    {
      ELLE_DEBUG("update %s's avatar", tr.second->peer_fullname().toStdString());
      tr.second->avatar_available();
    }
  }
}

void
MainPanel::user_status_changed(uint32_t uid,
                                      gap_UserStatus status)
{
  ELLE_TRACE_SCOPE("%s: user %s status changed to %s", *this, uid, status);

  // XXX: Do per user update.
  this->_transaction_list->reload();
}

void
MainPanel::_on_transaction_accepted(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: accept transaction %s", *this, tid);
  gap_accept_transaction(this->_state, tid);
}

void
MainPanel::_on_transaction_rejected(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: reject transaction %s", *this, tid);
  gap_reject_transaction(this->_state, tid);
}

void
MainPanel::_on_transaction_canceled(uint32_t tid)
{
  ELLE_TRACE_SCOPE("%s: cancel transaction %s", *this, tid);
  gap_cancel_transaction(this->_state, tid);
}

void
MainPanel::transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  ELLE_TRACE_SCOPE("transaction %s updated with status %s", id, status);
  g_panel->_transaction_cb(id, status);
}

void
MainPanel::_transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  if (gap_transaction_status(this->_state, id) <
      gap_transaction_waiting_accept)
    return; // Ignore early status because data may not be fully merged.

  if (this->_transactions.find(id) == this->_transactions.end())
  {
    ELLE_TRACE("new transaction: %s", id)
      this->add_transaction(this->_state, id);
  }
  else
  {
    ELLE_DEBUG("update transaction: %s", id)
      this->updateTransaction(this->_state, id);
  }
}

void
MainPanel::updateTransaction(gap_State* /* state */, uint32_t id)
{
  ELLE_TRACE_SCOPE("%s: update transaction %s", *this, id);

  ELLE_ASSERT(this->_transactions.at(id) != nullptr);
  auto const& transaction = *this->_transactions.at(id);

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

  for (auto widget: this->_transaction_list->widgets())
    widget->_update();
}

/*-------.
| Footer |
`-------*/
TransactionFooter*
MainPanel::footer()
{
  return static_cast<TransactionFooter*>(this->_footer);
}

void
MainPanel::print(std::ostream& stream) const
{
  stream << "MainPanel";
}
