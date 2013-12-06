#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>

#include <iostream>

#define MAX_TRANSAS 15

static TransactionPanel* g_panel = nullptr;
static gap_State* g_state = nullptr;

TransactionPanel::TransactionPanel(gap_State* state, QWidget* parent):
  Panel(new TransactionFooter, parent),
  _list(new ListWidget(this)),
  _state(state)
{
  this->footer()->setParent(this);

  // Register gap callback.
  g_panel = this;
  gap_transaction_callback(state, TransactionPanel::transaction_cb);

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
    // XXX: Add place holder.
    return;
  }

  auto iter = transactions_list.begin();
  for (uint32_t i = 0;
       iter != transactions_list.end() && i < MAX_TRANSAS;
       ++i, ++iter)
  {
    this->add_transaction(state, *iter, true);
  }
}

TransactionWidget*
TransactionPanel::add_transaction(gap_State* state,
                                  uint32_t tid,
                                  bool init)
{
  if (this->_transactions.find(tid) == this->_transactions.end())
    this->_transactions.emplace(tid, TransactionModel(state, tid));

  auto widget = new TransactionWidget(this->_transactions.at(tid));

  connect(widget, SIGNAL(on_transaction_accepted(uint32_t)),
          this, SLOT(_on_transaction_accepted(uint32_t)));
  connect(widget, SIGNAL(on_transaction_rejected(uint32_t)),
          this, SLOT(_on_transaction_rejected(uint32_t)));

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
  std::cerr << "accepted " << tid << std::endl;
  gap_accept_transaction(this->_state, tid);
}

void
TransactionPanel::_on_transaction_rejected(uint32_t tid)
{
  std::cerr << "reject " << tid << std::endl;
  gap_reject_transaction(this->_state, tid);
}

void
TransactionPanel::transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  if (status == gap_transaction_waiting_for_accept)
  {
    std::cerr << id << std::endl;
    g_panel->add_transaction(g_panel->_state, id);
  }
  else
    // std::cerr << this->_list->widgets().size() << "update transaction(s)" << std::endl;
    g_panel->updateTransaction(g_panel->_state, id);
}

void
TransactionPanel::updateTransaction(gap_State* state, uint32_t tid)
{
  // XXX:
  std::cerr << this->_list->widgets().size() << "update transaction(s)" << std::endl;
  for (auto widget: this->_list->widgets())
    widget->update();
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
TransactionPanel::on_list_resized()
{

}
