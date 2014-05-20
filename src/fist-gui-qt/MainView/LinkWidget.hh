#ifndef FIST_GUI_QT_MAINVIEW_LINKWIDGET_HH
# define FIST_GUI_QT_MAINVIEW_LINKWIDGET_HH

# include <QHBoxLayout>
# include <QLabel>

# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/Link.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/gui/Tooltip.hh>

namespace fist
{
  namespace mainview
  {
    class LinkWidget
      : public ListItem
    {
      typedef ListItem Super;
    public:
      LinkWidget(fist::model::Link const& model,
                 QWidget* parent = nullptr);
      virtual
      ~LinkWidget() = default;

      virtual
      QSize
      sizeHint() const override
      {
        auto size = this->_layout->minimumSize();
        return QSize(Super::sizeHint().width(), size.height());
      }

      void
      enterEvent(QEvent* event) override;

      void
      leaveEvent(QEvent* event) override;

      void
      paintEvent(QPaintEvent* event) override;

    private slots:
      void
      _update(QString const& status);

      void
      _on_progress_updated();

      void
      _on_status_updated();

      void
      _copy_link_to_clipboard();

      void
      _open_link();

      void
      trigger() override {}

    private:
      ELLE_ATTRIBUTE(fist::model::Link const&, model);
      ELLE_ATTRIBUTE(QHBoxLayout*, layout);
      ELLE_ATTRIBUTE(QLabel, picture);
      ELLE_ATTRIBUTE(QLabel, status);
      ELLE_ATTRIBUTE(QLabel, click_counter);
      ELLE_ATTRIBUTE(IconButton*, go_to_website);
      ELLE_ATTRIBUTE(IconButton*, copy_link);

    private:
      Q_OBJECT;
    };
  }
}


#endif
