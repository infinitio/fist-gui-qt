#include <QHBoxLayout>

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
  _list(nullptr),
  _state(state)
{
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  this->_list = new TransactionList;
  layout->addWidget(this->_list);
  layout->addWidget(this->_footer);

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

  auto iter = transactions_list.begin();
  uint32_t i = 0;
  for ( ; i < 15 && iter != transactions_list.end(); i += 1, iter++);
  for ( ; iter != transactions_list.begin(); iter--)
    addTransaction(state, *iter);
}

void
TransactionPanel::setFocus()
{
  this->_list->setFocus();
}

TransactionWidget*
TransactionPanel::addTransaction(gap_State* state, uint32_t tid)
{
  return this->_list->addTransaction(state, tid);
}

void
TransactionPanel::transaction_cb(uint32_t id, gap_TransactionStatus status)
{
  if (status == gap_transaction_waiting_for_accept)
    g_panel->addTransaction(g_panel->_state, id);
  else
    g_panel->_list->updateTransaction(g_panel->_state, id);
}

/*-------.
| Footer |
`-------*/
TransactionFooter*
TransactionPanel::footer()
{
  return static_cast<TransactionFooter*>(this->_footer);
}
