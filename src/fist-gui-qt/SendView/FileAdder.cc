#include <QPaintEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QEvent>

#include <elle/log.hh>

#include <fist-gui-qt/SendView/FileAdder.hh>
#include <fist-gui-qt/SendView/ui.hh>

ELLE_LOG_COMPONENT("barette");

namespace fist
{
  namespace sendview
  {
    FileAdder::FileAdder(QWidget* parent)
      : QWidget(parent)
    {
      this->setAcceptDrops(true);
    }

    void
    FileAdder::dropEvent(QDropEvent *event)
    {
      ELLE_TRACE_SCOPE("%s: drop", *this);

      if (event->mimeData()->hasUrls())
        for (auto const& url: event->mimeData()->urls())
          if (url.isLocalFile())
          {
            event->acceptProposedAction();
            ELLE_DEBUG("%s dropped", url);
            emit file_dropped(url);
          }
      emit dropped();
    }

    void
    FileAdder::paintEvent(QPaintEvent*)
    {
      QPainter painter(this);
      {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0xFA, 0xFA, 0xFA));
        painter.drawRect(0, 0, this->width(), this->height());
      }
    }

    EmptyFileAdder::EmptyFileAdder(QWidget* parent)
      : FileAdder(parent)
      , _icon(new QLabel(this))
      , _title(new QLabel(view::add_files::title::text, this))
      , _subtitle(new QLabel(view::add_files::subtitle::text, this))
      , _hover(false)
    {
      this->setFixedHeight(180);
      QVBoxLayout* layout = new QVBoxLayout(this);
      {
        layout->addStretch();
        {
          this->_icon->setPixmap(QPixmap{":/send/drop-files"});
          layout->addWidget(this->_icon, 0, Qt::AlignCenter);
        }
        {
          view::add_files::title::style(*this->_title);
          layout->addWidget(this->_title, 0, Qt::AlignCenter);
        }

        {
          view::add_files::subtitle::style(*this->_subtitle);
          layout->addWidget(this->_subtitle, 0, Qt::AlignCenter);
        }
        layout->addStretch();
      }
    }

    void
    EmptyFileAdder::hover(bool hover)
    {
      this->_hover = hover;
      this->repaint();
    }
    void
    EmptyFileAdder::dragEnterEvent(QDragEnterEvent * event)
    {
      this->hover(true);
      FileAdder::dragEnterEvent(event);
    }

    void
    EmptyFileAdder::enterEvent(QEvent * event)
    {
      this->hover(true);
      FileAdder::enterEvent(event);
    }

    void
    EmptyFileAdder::mouseReleaseEvent(QMouseEvent* event)
    {
      this->hover(false);
      emit this->clicked();
    }

    void
    EmptyFileAdder::dragLeaveEvent(QDragLeaveEvent * event)
    {
      this->hover(false);
      FileAdder::dragLeaveEvent(event);
    }

    void
    EmptyFileAdder::leaveEvent(QEvent* event)
    {
      this->hover(false);
      FileAdder::leaveEvent(event);
    }

    void
    EmptyFileAdder::paintEvent(QPaintEvent* event)
    {
      FileAdder::paintEvent(event);
      QPainter painter(this);
      {
        int m = 14;
        QRect rect{m, m, this->width() - m * 2, this->height() - m * 2};
        QPen p{QColor{0xBE,0xBE,0xBE}};
        if (this->_hover)
          painter.setBrush(QColor(0xFE, 0xFE, 0xFE));
        p.setWidth(1);
        p.setStyle(Qt::DashLine);
        painter.setPen(p);
        painter.drawRect(rect);
      }
    }

    NonEmptyFileAdder::NonEmptyFileAdder(QWidget* parent)
      : FileAdder(parent)
      , _attach(new IconButton(":/send/files", this))
      , _message(new QLabel(this))
      , _expanser(
        new fist::TwoStateIconButton(":/send/show-files",
                                     ":/send/hide-files",
                                     true,
                                     this,
                                     QSize(10, 10)))
    {
      this->setFixedHeight(45);
      QHBoxLayout* layout = new QHBoxLayout(this);
      {
        layout->addWidget(this->_attach);

        // Create a widget in order to calculate the height and fix it.
        layout->setAlignment(Qt::AlignVCenter);
        layout->setSpacing(6);
        layout->setContentsMargins(16, 0, 13, 0);
        layout->addWidget(this->_attach);
        layout->setSpacing(16);
        {
          view::file_adder::style(*this->_message);
          layout->addWidget(this->_message, 1);
        }
        layout->addWidget(this->_expanser);
      }
      this->_attach->installEventFilter(this);
      this->_message->installEventFilter(this);
    }

    void
    NonEmptyFileAdder::set_text(QString const& text,
                                bool save)
    {
      this->_message->setText(text);
      if (save)
        this->_old_text = text;
    }
    bool
    NonEmptyFileAdder::eventFilter(QObject *obj, QEvent *event)
    {
      if (event->type() == QEvent::Enter || event->type() == QEvent::DragEnter)
      {
        if (obj == this->_attach)
          this->_attach->set_pixmap(":/send/files-hover");
        else if (obj == this->_message)
        {
          this->_old_text = this->_message->text();
          this->set_text("Drop files or click to add more...", false);
        }
      }
      else if (event->type() == QEvent::Leave || event->type() == QEvent::DragLeave || event->type() == QEvent::WindowBlocked)
      {
        this->_attach->set_pixmap(":/send/files");
        if (obj == this->_message)
          this->_message->setText(this->_old_text);
      }
      else if (event->type() == QEvent::MouseButtonRelease)
      {
        emit this->clicked();
      }

      return FileAdder::eventFilter(obj, event);
    }
  }
}
