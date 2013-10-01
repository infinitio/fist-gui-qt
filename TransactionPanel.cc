#include <QHBoxLayout>

#include "Footer.hh"
#include "IconButton.hh"
#include "TransactionPanel.hh"
#include "TransactionWidget.hh"

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

TransactionPanel::TransactionPanel(gap_State* state):
  _list(nullptr)
{
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  this->_list = new TransactionList;
  layout->addWidget(this->_list);
  layout->addWidget(new TransactionFooter);

  auto trs = gap_transactions(state);

  for (uint32_t i = 0; trs[i] != 0; i += 1)
  {
    uint32_t uid;

    if (gap_self_id(state) == gap_transaction_recipient_id(state, trs[i]))
      uid = gap_transaction_sender_id(state, trs[i]);
    else
      uid = gap_transaction_recipient_id(state, trs[i]);

    if (std::find(_uids.begin(), _uids.end(), uid) == _uids.end())
    {
      _uids.push_back(uid);
      addTransaction(state, trs[i]);
    }
  }

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
