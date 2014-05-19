#ifndef FIST_GUI_QT_MAINVIEW_LINKS_HH
# define FIST_GUI_QT_MAINVIEW_LINKS_HH

# include <unordered_map>

# include <QSystemTrayIcon>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/model/Link.hh>
# include <fist-gui-qt/MainView/LinkWidget.hh>
# include <fist-gui-qt/State.hh>

namespace fist
{
  namespace mainview
  {
    class Links
      : public QWidget
      , public elle::Printable
    {
      typedef QWidget Super;
    public:
      Links(fist::State& state,
            QWidget* parent);

      void
      add_link(uint32_t id);

      void
      add_link(model::Link const& model);

    private:
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE(ListWidget*, link_list);
      typedef std::unordered_map<uint32_t, LinkWidget*> Widgets;
      ELLE_ATTRIBUTE(Widgets, widgets);
    private:
      Q_OBJECT;

      /*----------.
      | Printable |
      `----------*/
      void
      print(std::ostream& stream) const override;
    };
  }
}

#endif
