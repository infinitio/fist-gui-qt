#ifndef FIST_GUI_QT_SEND_VIEW_NOSEARCHRESULTWIDGET_HH
# define FIST_GUI_QT_SEND_VIEW_NOSEARCHRESULTWIDGET_HH

# include <QVBoxLayout>

# include <elle/attribute.hh>

# include <fist-gui-qt/ListItem.hh>

namespace fist
{
  namespace sendview
  {
    class NoSearchResultWidget
      : public ListItem
    {
      typedef ListItem Super;
    public:
      NoSearchResultWidget(QWidget* parent = nullptr);
      virtual
      ~NoSearchResultWidget() = default;

      void
      trigger() override
      {}

      QSize
      sizeHint() const override;

      ELLE_ATTRIBUTE(QVBoxLayout*, layout);
    };
  }
}

#endif
