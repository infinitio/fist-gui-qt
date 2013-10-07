#include <QHBoxLayout>

#include "Footer.hh"
#include "IconButton.hh"
#include "TransactionPanel.hh"
#include "TransactionWidget.hh"

#define MAX_TRANSAS 15

static TransactionPanel* g_panel = nullptr;

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

  auto trs = gap_transactions(state);

  for (uint32_t i = 0; trs[i] != 0 && i <= MAX_TRANSAS; i += 1)
    addTransaction(state, trs[i]);

  gap_transactions_free(trs);
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
  std::cout << "transaction_cb appellee " << status << std::endl;

  if (status == gap_transaction_waiting_for_accept)
    g_panel->addTransaction(g_panel->_state, id);
  else
    g_panel->_list->updateTransaction(g_panel->_state, id);
}


TransactionPanel::~TransactionPanel()
{
  std::cerr << "Error: TransactionPanel destructor called" << std::endl;
}
