#include <QHBoxLayout>

#include "Footer.hh"
#include "IconButton.hh"
#include "TransactionPanel.hh"
#include "TransactionWidget.hh"

#define MAX_TRANSAS 15

static TransactionPanel* g_panel = nullptr;
static gap_State* g_state = nullptr;

class TransactionFooter:
  public Footer
{
public:
  TransactionFooter()
  {
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 5);
    layout->addWidget(new IconButton(QPixmap(":/icons/gear.png"), true));
    layout->addItem(new QSpacerItem(0, 0,
                                    QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Minimum));
    layout->addWidget(new IconButton(QPixmap(":/icons/arrows.png"), true));
  }
};

TransactionPanel::TransactionPanel(gap_State* state, QWidget* parent):
  QWidget(parent),
  _list(nullptr),
  _state(state)
{
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  this->_list = new TransactionList;
  layout->addWidget(this->_list);
  layout->addWidget(new TransactionFooter);

  // Register gap callback.
  g_panel = this;
  gap_transaction_callback(state, TransactionPanel::transaction_cb);

  //TODO: kill this abomination with a gap_transactions already sorted.
  uint32_t* trs = gap_transactions(state);

  g_state = state;
  struct trs_compare
  {
    bool
    operator() (const uint32_t& first, const uint32_t& second)
    {
      double first_mtime = gap_transaction_mtime(g_state, first);
      double second_mtime = gap_transaction_mtime(g_state, second);

      return first_mtime > second_mtime;
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
