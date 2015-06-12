#ifndef FIST_GUI_QT_SETTINGS_SCREENSHOT_HH
# define FIST_GUI_QT_SETTINGS_SCREENSHOT_HH

# include <QWidget>
# include <QLabel>
# include <QLineEdit>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/gui/EnterKeySequence.hh>
# include <elle/attribute.hh>

namespace fist
{
  namespace prefs
  {
    class ScreenShot
      : public QWidget
    {
      Q_OBJECT

    public:
      typedef QWidget Super;

    public:
      explicit
      ScreenShot(fist::State& state,
                 QWidget *parent = nullptr);

      ELLE_ATTRIBUTE(fist::State&, state);

    private:
      bool
      eventFilter(QObject *obj, QEvent *event) override;

    private slots:
      void
      _change_fullscreen_shortcut(QString const&);

      void
      _change_region_shortcut(QString const&);

      void
      _no_modifiers();

    private:
      void
      _set_message(QString const& message);

      bool
      _validate_key_sequence(QString const& c,
                             QKeySequence const& other_sequence);

    private:
      ELLE_ATTRIBUTE(gui::EnterKeySequence*, fullscreen);
      ELLE_ATTRIBUTE(gui::EnterKeySequence*, region);
      ELLE_ATTRIBUTE(QLabel*, message);
    };
  }
}

#endif
