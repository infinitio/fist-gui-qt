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
      : Super(parent, Qt::transparent, false)
      , _model(model)
      , _layout(new QHBoxLayout(this))
      , _picture()
      , _text_style(view::links::file::style)
      , _name(this->_model.name())
      , _status()
      , _click_counter()
      , _cancel_link(new IconButton(QPixmap(":/link/delete")))
      , _go_to_website(new IconButton(QPixmap(":/link/share")))
      , _copy_link(new IconButton(QPixmap(":/link/clipboard")))
      , _progress_timer(nullptr)
      , _update_progress_interval(1000)
      , _smooth_progress(0.0f)
      , _progress_animation(new QPropertyAnimation(this, "smooth_progress"))
    {
      connect(&this->_model, SIGNAL(status_updated()),
              this, SLOT(_on_status_updated()));
      connect(&this->_model, SIGNAL(click_count_updated()),
              this, SLOT(_on_status_updated()));
      connect(&this->_model, SIGNAL(payment_required()),
              this, SLOT(hide()));
      this->_layout->setContentsMargins(12, 12, 12, 12);
      this->_layout->setSpacing(5);
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
          this->_name.setToolTip(this->_model.name());
          this->_text_style(this->_name);
          vlayout->addWidget(&this->_name);
        }
        {
          view::links::status::style(this->_status);
          vlayout->addWidget(&this->_status);
        }
        vlayout->addStretch();
        this->_layout->addLayout(vlayout, 1);
      }
      {
        auto* hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0, 0, 0, 0);
        hlayout->setSpacing(3);
        hlayout->addStretch();
        hlayout->addWidget(this->_go_to_website);
        this->_go_to_website->setToolTip("Open the link");
        hlayout->addSpacing(5);
        hlayout->addWidget(this->_copy_link);
        this->_copy_link->setToolTip("Copy the link to clipboard");
        hlayout->addSpacing(5);
        hlayout->addWidget(this->_cancel_link);
        this->_cancel_link->installEventFilter(this);
        this->_cancel_link->setToolTip("Cancel");
        {
          auto* vlayout = new QVBoxLayout;
          vlayout->addStretch();
          vlayout->setContentsMargins(0, 0, 0, 0);
          view::links::counter::style(this->_click_counter);
          this->_click_counter.setStyleSheet(
            "border: 5px solid rgb(204,204,204); border-radius: 5px; background-color: rgb(204,204,204);");
          vlayout->addStretch();
          vlayout->addWidget(&this->_click_counter);
          vlayout->addStretch();
          hlayout->addLayout(vlayout);
        }
        this->_layout->addLayout(hlayout);
      }
      connect(this->_go_to_website, SIGNAL(clicked()),
              this, SLOT(_open_link()));
      connect(this->_cancel_link, SIGNAL(clicked()),
              this, SLOT(_cancel()));
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
      ELLE_TRACE_SCOPE("%s: on status updated", *this);
      this->_picture.setPixmap(gui::icon_provider().icon(this->_model.name()).scaled(42, 42));
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
      ELLE_TRACE_SCOPE("%s: update (%s)", *this, status);
      this->_status.setText(status);
      this->_click_counter.setText(
        QString("%1").arg(this->_model.click_count()));
      if (this->_model.unavailable())
      {
        this->_text_style = view::links::file::failed_style;
      }
      else if (this->_model.status() == gap_transaction_finished)
      {
        this->_text_style = view::links::file::style;
      }

      if ((this->_model.status() == gap_transaction_transferring) &&
          (this->_progress_timer == nullptr))
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
      if (this->_name.text().isEmpty())
        this->_name.setText(this->_model.name());
      this->_text_style(this->_name);
      this->repaint();
    }

    void
    LinkWidget::_cancel()
    {
      ELLE_TRACE_SCOPE("%s: cancel/delete transaction", *this);
      this->_first_click = !this->_first_click;
      if (this->_first_click)
      {
        this->_go_to_website->hide();
        this->_copy_link->hide();
        QToolTip::showText(
          this->_cancel_link->mapToGlobal(
            QPoint(0, -this->_cancel_link->height())),
          "Click again to cancel");
      }
      else
      {
        emit transaction_deleted(this->_model.id());
      }
    }

    bool
    LinkWidget::eventFilter(QObject *obj, QEvent *event)
    {
      if (obj == this->_cancel_link)
      {
        if (event->type() == QEvent::Leave)
        {
          this->_go_to_website->show();
          this->_copy_link->show();
          this->_first_click = false;
        }
      }
      return Super::eventFilter(obj, event);
    }

    void
    LinkWidget::_progress_updated()
    {
      auto old_progress = this->_progress_animation->endValue();
      auto new_progress = this->_model.progress();
      this->_update(
        QString::fromStdString(
          elle::sprintf("Uploading... (%s%%)", int(new_progress * 100))));
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
      auto style = this->_text_style;
      style.color(style.color().darker(20 + 50 * this->_smooth_progress));
      style(this->_name);
      this->repaint();
    }

    void
    LinkWidget::enterEvent(QEvent* e)
    {
      Super::enterEvent(e);
      if (this->_model.unavailable())
        return;
      this->_cancel_link->show();
      this->_go_to_website->show();
      this->_copy_link->show();
      this->_click_counter.hide();
    }

    void
    LinkWidget::leaveEvent(QEvent* e)
    {
      Super::leaveEvent(e);
      this->_cancel_link->hide();
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
