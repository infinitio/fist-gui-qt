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

class MainPanel:
  public Panel
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
  ELLE_ATTRIBUTE_R(fist::mainview::Transactions*, transactions);
  ELLE_ATTRIBUTE_R(fist::mainview::Links*, links);

private:
  void
  keyPressEvent(QKeyEvent* event) override;

public:
  TransactionFooter*
  footer();

signals:
  void
  systray_message(QString const& title,
                  QString const& body,
                  QSystemTrayIcon::MessageIcon = QSystemTrayIcon::Information);

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
