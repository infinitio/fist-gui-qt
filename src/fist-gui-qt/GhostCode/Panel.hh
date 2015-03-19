#ifndef FIST_GUI_QT_GHOST_CODE_PANEL_HH
# define FIST_GUI_QT_GHOST_CODE_PANEL_HH

# include <QLabel>
# include <QLineEdit>

# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/GhostCode/Footer.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace ghost_code
  {
    class Panel
      : public ::Panel
    {
    public:
      Panel(fist::State& state);

      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(QMovie*, loading_icon);
      ELLE_ATTRIBUTE(QLabel*, loading);
      ELLE_ATTRIBUTE(QLabel*, message);
      ELLE_ATTRIBUTE(QLineEdit*, field);
    private slots:
      void
      _text_udpated(QString const& text);

      void
      _use_code();

      void
      _ghost_code_result(gap_Status status);

    /*-------.
    | Footer |
    `-------*/
    public:
      Footer*
      footer() override;

      void
      showEvent(QShowEvent* event) override;

      void
      hideEvent(QHideEvent* event) override;

      void
      keyPressEvent(QKeyEvent* event) override;

    private:
      Q_OBJECT;
    };
  }
}

#endif
