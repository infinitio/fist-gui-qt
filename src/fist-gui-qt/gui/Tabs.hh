#ifndef FIST_GUI_QT_GUI_TABS_HH
# define FIST_GUI_QT_GUI_TABS_HH

# include <memory>
# include <vector>

# include <QWidget>
# include <QVector>
# include <QHBoxLayout>

# include <elle/attribute.hh>


# include <fist-gui-qt/gui/Tab.hh>

class Tabber:
  public QWidget
{
  typedef QWidget Super;
public:
Tabber(QWidget* parent = nullptr);

  void
  add_tab(QString const& name,
          QVector<QWidget*> const& widgets = {});

  void
  add_tab(QString const& name,
          QWidget* widgets);

  void
  active_tab(Tab& tab);

  bool
  is_active_tab(Tab& tab) const;

  void
  remove_tab(QString const& name);

  QSize
  sizeHint() const override
  {
    return QSize(320, Super::sizeHint().height());
  }

  ELLE_ATTRIBUTE(QHBoxLayout*, layout);
  typedef std::vector<std::unique_ptr<Tab>> Tabs;
  ELLE_ATTRIBUTE_R(Tabs, tabs);
  ELLE_ATTRIBUTE(Tab*, active_tab);
};

#endif
