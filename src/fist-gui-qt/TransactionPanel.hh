#ifndef TRANSACTIONPANEL_HH
# define TRANSACTIONPANEL_HH

# include <set>
# include <unordered_map>
# include <algorithm>

# include <QScrollArea>
# include <QFrame>

# include <elle/attribute.hh>

# include <surface/gap/gap.hh>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/TransactionFooter.hh>
# include <fist-gui-qt/MainView/Transactions.hh>
# include <fist-gui-qt/MainView/Links.hh>
# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/gui/TabWidget.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/ModeSelector.hh>

class MainPanel
  : public Panel
  , public fist::ModeSelector
{
public:
  MainPanel(fist::State& state,
            QWidget* parent = nullptr);

public:
  void
  _on_show() override;

private:
  fist::State& _state;
  fist::gui::TabWidget* _tabs;
private:
  ELLE_ATTRIBUTE_R(fist::mainview::Transactions*, transactions);
  ELLE_ATTRIBUTE_R(fist::gui::Tab*, transactions_tab);
  ELLE_ATTRIBUTE_R(fist::mainview::Links*, links);
  ELLE_ATTRIBUTE_R(fist::gui::Tab*, links_tab);

private:
  void
  keyPressEvent(QKeyEvent* event) override;

public:
  TransactionFooter*
  footer();

private:
  void
  _mode_implementation() override;

private slots:
  void
  p2p_mode();

  void
  link_mode();

private:
  /*----------.
  | Printable |
  `----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

#endif
