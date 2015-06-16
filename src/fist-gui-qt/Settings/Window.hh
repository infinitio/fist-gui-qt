#ifndef FIST_GUI_QT_SETTINGS_WINDOW_HH
# define FIST_GUI_QT_SETTINGS_WINDOW_HH

# include <iostream>

# include <QCheckBox>
# include <QDialog>
# include <QLabel>
# include <QLineEdit>
# include <QTabBar>
# include <QTabWidget>

# include <fist-gui-qt/State.hh>

# include <elle/attribute.hh>

namespace fist
{
  namespace prefs
  {

    class TabBar
      : public QTabBar
    {
    public:
      TabBar(QWidget* parent);

    private:
      void
      paintEvent(QPaintEvent* event) override;
    };

    class TabWidget:
      public QTabWidget
    {
    public:
      TabWidget(QWidget* parent)
        : QTabWidget(parent)
      {
        this->setTabBar(new TabBar(this));
      }

    private:
      void
      paintEvent(QPaintEvent* event) override;
    };

    class Window:
      public QDialog
    {
      Q_OBJECT;

    public:
      explicit
      Window(fist::State& state,
             QWidget* parent = nullptr);

    private:
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(TabWidget*, tabWidget);
    };
  }
}

#endif
