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
#include <fist-gui-qt/globals.hh>

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
      , _status("branle chiche")
      , _download_counter("32")
      , _go_to_website(new IconButton(QPixmap(":/buttons/share.png")))
      , _copy_link(new IconButton(QPixmap(":/buttons/clipboard.png")))
    {
      this->_layout->setContentsMargins(12, 12, 12, 12);
      this->_layout->setSpacing(10);
      {
        QFileIconProvider icon_provider;
        if (QFileInfo(this->_model.name()).isDir())
          this->_picture.setPixmap(icon_provider.icon(QFileIconProvider::Folder).pixmap(42));
        else
          this->_picture.setPixmap(icon_provider.icon(this->_model.name()).pixmap(42));
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
          view::links::counter::style(this->_download_counter);
          this->_download_counter.setStyleSheet(
            "border: 3px solid rgb(204,204,204); border-radius: 8px; background-color: rgb(204,204,204);");
        }

        vlayout->addWidget(&this->_download_counter);
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

      this->leaveEvent(nullptr);
      this->_on_progress_updated();
    }

    void
    LinkWidget::_on_progress_updated()
    {
      this->_on_status_updated(QString("Uploading... (%1%)").arg(this->_model.progress() * 100));
    }

    void
    LinkWidget::_on_status_updated(QString const& status)
    {
      this->_status.setText(status);
    }

    void
    LinkWidget::enterEvent(QEvent*)
    {
      this->_go_to_website->show();
      this->_copy_link->show();
      this->_download_counter.hide();
    }

    void
    LinkWidget::leaveEvent(QEvent*)
    {
      this->_go_to_website->hide();
      this->_copy_link->hide();
      this->_download_counter.show();
    }

    void
    LinkWidget::paintEvent(QPaintEvent* event)
    {
      Super::paintEvent(event);
      int line_height = 2;

      QPainter painter(this);
      static QColor color(0x50, 0xD3, 0xED);
      painter.setPen(color);
      painter.setBrush(color);
      painter.drawRect(0, this->height() - line_height, this->width() * this->_model.progress(), line_height);
    }

    void
    LinkWidget::_copy_link_to_clipboard()
    {
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(this->_model.url().toString());
      QToolTip::showText(this->_copy_link->mapToGlobal(QPoint()), "Link copied to your clipboard");
    }

    void
    LinkWidget::_open_link()
    {
      QDesktopServices::openUrl(this->_model.url());
    }

  }
}
