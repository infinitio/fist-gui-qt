#include <fist-gui-qt/Footer.hh>
#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionPanel.hh>
#include <fist-gui-qt/TransactionWidget.hh>
#include <fist-gui-qt/TextListItem.hh>

#include <elle/assert.hh>

MainPanel::MainPanel(fist::State& state,
                     QWidget* parent)
  : Panel(new TransactionFooter(state), parent)
  , _state(state)
  , _tabs(new fist::gui::TabWidget(this))
  , _transactions(new fist::mainview::Transactions(state, this))
  , _transactions_tab(this->_tabs->add_tab("TRANSFERS", {this->_transactions}))
  , _links(new fist::mainview::Links(state, this))
  , _links_tab(this->_tabs->add_tab("LINKS", {this->_links}))
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
    &(this->_state.acceptable_transactions()), SIGNAL(size_changed(size_t)),
    this->_transactions_tab, SLOT(on_notification_count_changed(size_t)));
  this->_transactions_tab->on_notification_count_changed(
    this->_state.acceptable_transactions().size());
  connect(this->_transactions_tab, SIGNAL(activated()), this, SLOT(p2p_mode()));
  connect(&this->_state, SIGNAL(active_links_changed(size_t)),
          this->_links_tab, SLOT(on_notification_count_changed(size_t)));
  this->_links_tab->on_notification_count_changed(this->_state.active_links());
  connect(this->_links_tab, SIGNAL(activated()), this, SLOT(link_mode()));
}

void
MainPanel::_mode_implementation()
{
  switch (this->mode())
  {
    case fist::Mode::p2p:
      this->_transactions_tab->click();
      break;
    case fist::Mode::link:
      this->_links_tab->click();
      break;
  }
  this->footer()->mode(this->mode());
}

void
MainPanel::p2p_mode()
{
  this->mode(fist::Mode::p2p);
}

void
MainPanel::link_mode()
{
  this->mode(fist::Mode::link);
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
MainPanel::showEvent(QShowEvent* event)
{
  this->_tabs->show();
  Super::showEvent(event);
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
