#ifndef FIST_GUI_QT_PROXY_PANEL_HH
# define FIST_GUI_QT_PROXY_PANEL_HH

# include <QCheckBox>

# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/fwd.hh>

# include <QPushButton>
# include <QLineEdit>
# include <QLabel>
# include <QDialog>

namespace fist
{
  namespace proxy
  {
    class Window
      : public QDialog
    {
    public:
      Window(State& state,
             QWidget* parent);

    private slots:
      void
      _update(int = 0);

      void
      _apply();

    private:
      ELLE_ATTRIBUTE_R(QCheckBox*, activated);
      ELLE_ATTRIBUTE_R(State&, state);
      ELLE_ATTRIBUTE_R(QLineEdit*, username);
      ELLE_ATTRIBUTE_R(QLineEdit*, password);
      ELLE_ATTRIBUTE_R(QLineEdit*, host);
      ELLE_ATTRIBUTE_R(QLineEdit*, port);
      ELLE_ATTRIBUTE_R(std::vector<QLineEdit*>, entries);
      ELLE_ATTRIBUTE_R(QPushButton*, validate);

    signals:
      void
      done();

    private:
      Q_OBJECT;
    };
  }
}


#endif
