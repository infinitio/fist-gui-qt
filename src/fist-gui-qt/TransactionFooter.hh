#ifndef FIST_GUI_QT_TRANSACTIONFOOTER_HH
# define FIST_GUI_QT_TRANSACTIONFOOTER_HH

# include <QProgressBar>
# include <QLabel>

# include <elle/attribute.hh>

# include <fist-gui-qt/Footer.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ModeSelector.hh>

class TransactionFooter
  : public Footer
  , public fist::ModeSelector
{
  // As his parent footer, ownership is managed somewhere else.
public:
  TransactionFooter(fist::State& state);

private:
  void
  _mode_implementation() override;

private slots:
  void
  _account_updated();

private:
  bool
  eventFilter(QObject *obj, QEvent *event) override;

private:
  ELLE_ATTRIBUTE(fist::State&, state);
  ELLE_ATTRIBUTE_R(IconButton*, menu);
  ELLE_ATTRIBUTE_R(QProgressBar*, usage_bar);
  ELLE_ATTRIBUTE_R(QLabel*, usage_caption);
  ELLE_ATTRIBUTE_R(IconButton*, send);

private:
  Q_OBJECT;
};

#endif
