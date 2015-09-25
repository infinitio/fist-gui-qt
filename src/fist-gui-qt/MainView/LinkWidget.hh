#ifndef FIST_GUI_QT_MAINVIEW_LINKWIDGET_HH
# define FIST_GUI_QT_MAINVIEW_LINKWIDGET_HH

# include <QHBoxLayout>
# include <QLabel>
# include <QPropertyAnimation>
# include <QTimer>

# include <fist-gui-qt/ListItem.hh>
# include <fist-gui-qt/model/Link.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/gui/Tooltip.hh>
# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/globals.hh>

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
      _progress_updated();

      void
      _on_status_updated();

      void
      _copy_link_to_clipboard();

      void
      _open_link();

      void
      trigger() override {}

      void
      _cancel();

    private:
      void
      _update_cancel_tooltip();

    signals:
      void
      transaction_canceled(uint32_t);

      void
      transaction_deleted(uint32_t);

    private:
      bool
      eventFilter(QObject *obj,
                  QEvent *event) override;

    private:
      ELLE_ATTRIBUTE(fist::model::Link const&, model);
      ELLE_ATTRIBUTE(QHBoxLayout*, layout);
      ELLE_ATTRIBUTE(QLabel, picture);
      ELLE_ATTRIBUTE(style::Text, text_style);
      ELLE_ATTRIBUTE(QLabel, name);
      ELLE_ATTRIBUTE(QLabel, status);
      ELLE_ATTRIBUTE(QLabel, click_counter);
      ELLE_ATTRIBUTE(IconButton*, cancel_link);
      ELLE_ATTRIBUTE(bool, already_clicked);
      ELLE_ATTRIBUTE(IconButton*, go_to_website);
      ELLE_ATTRIBUTE(IconButton*, copy_link);
      ELLE_ATTRIBUTE(std::unique_ptr<QTimer>, progress_timer);
      ELLE_ATTRIBUTE(int, update_progress_interval);

    public:
      Q_PROPERTY(float smooth_progress
                 READ smooth_progress
                 WRITE _set_smooth_progress);
    private:
      ELLE_ATTRIBUTE_R(float, smooth_progress);
    private:
      void
      _set_smooth_progress(float);
      ELLE_ATTRIBUTE(QPropertyAnimation*, progress_animation);

    private:
      Q_OBJECT;
    };
  }
}


#endif
