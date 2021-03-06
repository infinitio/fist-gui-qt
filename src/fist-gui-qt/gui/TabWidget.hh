#ifndef FIST_GUI_QT_GUI_TABWIDGET_HH
# define FIST_GUI_QT_GUI_TABWIDGET_HH

# include <memory>
# include <vector>

# include <QHBoxLayout>
# include <QWidget>
# include <QVector>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/gui/Tab.hh>

namespace fist
{
  namespace gui
  {
    // TabWidget is a simpler version of the QTabWidget that just manipulate
    // visibility of a bunch of widget
    class TabWidget
      : public QWidget
      , public elle::Printable
    {
      typedef QWidget Super;
    public:
      TabWidget(QWidget* parent = nullptr);

      // Add a tab associated with a list a widgets.
      Tab*
      add_tab(QString const& name,
              QVector<QWidget*> const& widgets = QVector<QWidget*>{});

      // Add a tab associated with one widget.
      Tab*
      add_tab(QString const& name,
              QWidget* widgets);

      // Make the tab active (makes every widgets visible).
      void
      activate_tab(Tab& tab);

      // Make the first tab active.
      void
      activate_first();

      // Get the index of the current tab.
      int
      _active_tab_index() const;

      // Activate next tab.
      void
      activate_next();

      // Activate next tab.
      void
      activate_previous();

      bool
      is_active_tab(Tab& tab) const;

      // Remove a tab, activating the previous one, or hiding the TabWidget if
      // no tab remains.
      // XXX: Not implemented yet.
      void
      remove_tab(QString const& name);

      void
      showEvent(QShowEvent* event) override;

      QSize
      sizeHint() const override;

    private:
      void
      print(std::ostream& stream) const override;

    private:
      ELLE_ATTRIBUTE(QHBoxLayout*, layout);
      typedef std::vector<std::unique_ptr<Tab>> Tabs;
      ELLE_ATTRIBUTE_R(Tabs, tabs);
      ELLE_ATTRIBUTE(int, index);
      ELLE_ATTRIBUTE(Tab*, active_tab);
    };
  }
}
#endif
