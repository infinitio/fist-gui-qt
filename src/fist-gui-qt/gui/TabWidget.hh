#ifndef FIST_GUI_QT_GUI_TABWIDGET_HH
# define FIST_GUI_QT_GUI_TABWIDGET_HH

# include <memory>
# include <vector>

# include <QHBoxLayout>
# include <QWidget>
# include <QVector>

# include <elle/attribute.hh>


# include <fist-gui-qt/gui/Tab.hh>

namespace fist
{
  namespace gui
  {
    // TabWidget is a simpler version of the QTabWidget that just manipulate
    // visibility of a bunch of widgets.
    class TabWidget:
      public QWidget
    {
      typedef QWidget Super;
    public:
      TabWidget(QWidget* parent = nullptr);

      // Add a tab associated with a list a widgets.
      void
      add_tab(QString const& name,
              QVector<QWidget*> const& widgets = {});

      // Add a tab associated with one widget.
      void
      add_tab(QString const& name,
              QWidget* widgets);

      // Make the tab active (Make every widgets visible).
      void
      active_tab(Tab& tab);

      bool
      is_active_tab(Tab& tab) const;

      // Remove a tab, activating the previous one, or hiding the TabWidget if
      // no tab remains.
      // XXX: Not implemented yet.
      void
      remove_tab(QString const& name);

      QSize
      sizeHint() const override;

      ELLE_ATTRIBUTE(QHBoxLayout*, layout);
      typedef std::vector<std::unique_ptr<Tab>> Tabs;
      ELLE_ATTRIBUTE_R(Tabs, tabs);
      ELLE_ATTRIBUTE(Tab*, active_tab);
    };
  }
}
#endif
