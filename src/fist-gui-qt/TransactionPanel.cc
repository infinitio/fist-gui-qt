#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TextListItem.hh>

#include <elle/assert.hh>

MainPanel::MainPanel(fist::State& state,
                     QWidget* parent)
  : Panel(new TransactionFooter, parent)
  , _state(state)
  , _tabs(new fist::gui::TabWidget(this))
  , _transactions(new fist::mainview::Transactions(state, this))
  , _links(new fist::mainview::Links(state, this))
{
  connect(&this->_state, SIGNAL(new_transaction(uint32_t)),
          this->_transactions, SLOT(add_transaction(uint32_t)));
  connect(&this->_state, SIGNAL(transaction_updated(uint32_t)),
          this->_transactions, SLOT(on_transaction_updated(uint32_t)));
  connect(&this->_state, SIGNAL(new_link(uint32_t)),
          this->_links, SLOT(add_link(uint32_t)));
  connect(&this->_state, SIGNAL(link_updated(uint32_t)),
          this->_links, SLOT(on_link_updated(uint32_t)));
  connect(
    this->_transactions,
    SIGNAL(systray_message(fist::SystrayMessageCarrier const&)),
    this, SIGNAL(systray_message(fist::SystrayMessageCarrier const&)));
  connect(
    this->_links,
    SIGNAL(systray_message(fist::SystrayMessageCarrier const&)),
    this, SIGNAL(systray_message(fist::SystrayMessageCarrier const&)));

  auto* transfer_tab = this->_tabs->add_tab("TRANSFERS", {this->_transactions});
  connect(
    &this->_state, SIGNAL(acceptable_transactions_changed(size_t)),
    transfer_tab, SLOT(on_notification_count_changed(size_t)));
  transfer_tab->on_notification_count_changed(this->_state.acceptable_transactions());

  auto* link_tab = this->_tabs->add_tab("LINKS", {this->_links});
  connect(&this->_state, SIGNAL(active_links_changed(unsigned int)),
          link_tab, SLOT(on_notification_count_changed(unsigned int)));
  link_tab->on_notification_count_changed(this->_state.active_links());
}

void
MainPanel::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Right)
  {
    this->_tabs->activate_next();
  }
  if (event->key() == Qt::Key_Left)
  {
    this->_tabs->activate_previous();
  }
}

void
MainPanel::_on_show()
{
  this->_tabs->show();
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
