#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QFile>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QToolTip>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <fist-gui-qt/MainView/LinkWidget.hh>
#include <fist-gui-qt/gui/IconProvider.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.MainView.LinkWidget");

namespace fist
{
  namespace mainview
  {
    LinkWidget::LinkWidget(fist::model::Link const& model,
                           QWidget* parent)
      : Super(parent)
      , _model(model)
      , _layout(new QHBoxLayout(this))
      , _picture()
      , _status()
      , _click_counter()
      , _go_to_website(new IconButton(QPixmap(":/buttons/share.png")))
      , _copy_link(new IconButton(QPixmap(":/buttons/clipboard.png")))
      , _progress_timer(nullptr)
      , _update_progress_interval(1000)
      , _smooth_progress(0.0f)
      , _progress_animation(new QPropertyAnimation(this, "smooth_progress"))
    {
      connect(&this->_model, SIGNAL(status_updated()),
              this, SLOT(_on_status_updated()));

      this->_layout->setContentsMargins(12, 12, 12, 12);
      this->_layout->setSpacing(10);
      {
        this->_picture.setPixmap(gui::icon_provider().icon(this->_model.name()).scaled(42, 42));
        this->_layout->addWidget(&this->_picture);
      }
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->addStretch();
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(5);
        {
          auto* name = new QLabel(this->_model.name(), this);
          name->setToolTip(this->_model.name());
          view::links::file::style(*name);
          vlayout->addWidget(name);
        }
        {
          view::links::status::style(this->_status);
          vlayout->addWidget(&this->_status);
        }
        vlayout->addStretch();
        this->_layout->addLayout(vlayout, 1);
      }
      {
        auto* vlayout = new QVBoxLayout;
        vlayout->setContentsMargins(0, 0, 0, 0);
        vlayout->setSpacing(0);
        vlayout->addStretch();
        {
          view::links::counter::style(this->_click_counter);
          this->_click_counter.setStyleSheet(
            "border: 5px solid rgb(204,204,204); border-radius: 8px; background-color: rgb(204,204,204);");
        }

        vlayout->addWidget(&this->_click_counter);
        vlayout->addWidget(this->_go_to_website);
        this->_go_to_website->setToolTip("Open the link");
        vlayout->addSpacing(5);
        vlayout->addWidget(this->_copy_link);
        this->_copy_link->setToolTip("Copy the link to clipboard");
        vlayout->addStretch();
        this->_layout->addLayout(vlayout);
      }

      connect(this->_go_to_website, SIGNAL(clicked()),
              this, SLOT(_open_link()));
      connect(this->_copy_link, SIGNAL(clicked()),
              this, SLOT(_copy_link_to_clipboard()));

      {
        this->_progress_animation->setDuration(this->_update_progress_interval);
        this->_progress_animation->setEasingCurve(QEasingCurve::Linear);
        this->_progress_animation->setEndValue(0.0f);

      }

      this->leaveEvent(nullptr);
      this->_on_status_updated();
    }

    void
    LinkWidget::_on_status_updated()
    {
      if (this->_model.is_finished())
        this->_update(pretty_date(this->_model.mtime()));
      else
      {
        this->_update(
          QString("Uploading... (%1%)").arg(this->_model.progress() * 100));
      }
    }

    void
    LinkWidget::_update(QString const& status)
    {
      this->_status.setText(status);
      this->_click_counter.setText(
        QString("%1").arg(this->_model.click_count()));

      if (!this->_model.is_finished() && this->_progress_timer == nullptr)
      {
        this->_progress_timer.reset(new QTimer);
        this->_progress_timer->setInterval(this->_update_progress_interval);
        connect(this->_progress_timer.get(), SIGNAL(timeout()),
                this, SLOT(_progress_updated()));
        this->_progress_timer->start(this->_update_progress_interval);
      }
      else if (this->_model.is_finished() && this->_progress_timer != nullptr)
      {
        this->_progress_timer.reset();
      }
    }

    void
    LinkWidget::_progress_updated()
    {
      auto old_progress = this->_progress_animation->endValue();
      auto new_progress = this->_model.progress();
      this->_update(
        QString("Uploading... (%1%)").arg(new_progress * 100));
      ELLE_DEBUG_SCOPE("update progress from %s to %s",
                       old_progress, new_progress);
      this->_progress_animation->stop();
      this->_progress_animation->setStartValue(old_progress);
      this->_progress_animation->setEndValue(new_progress);
      this->_progress_animation->start();
      this->update();
    }

    void
    LinkWidget::_set_smooth_progress(float progress)
    {
      this->_smooth_progress = progress;
      auto style = view::links::status::style;
      style.color().darker(50 + 50 * this->_smooth_progress);
      style(this->_status);
      this->repaint();
    }

    void
    LinkWidget::enterEvent(QEvent*)
    {
      this->_go_to_website->show();
      this->_copy_link->show();
      this->_click_counter.hide();
    }

    void
    LinkWidget::leaveEvent(QEvent*)
    {
      this->_go_to_website->hide();
      this->_copy_link->hide();
      this->_click_counter.show();
    }

    void
    LinkWidget::paintEvent(QPaintEvent* event)
    {
      Super::paintEvent(event);
      if (!this->_model.is_finished())
      {
        static int line_height = 2;
        QPainter painter(this);
        static QColor color(0x50, 0xD3, 0xED);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawRect(0, this->height() - line_height,
                         this->width() * this->_smooth_progress, line_height);
      }
    }

    void
    LinkWidget::_copy_link_to_clipboard()
    {
      ELLE_TRACE_SCOPE("%s: copy link %s to clipboard", *this, this->_model.url());
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(this->_model.url().toString());
      QToolTip::showText(this->_copy_link->mapToGlobal(QPoint()), "Link copied to your clipboard");
    }

    void
    LinkWidget::_open_link()
    {
      ELLE_TRACE_SCOPE("%s: open link %s", *this, this->_model.url());
      QDesktopServices::openUrl(this->_model.url());
    }

  }
}
