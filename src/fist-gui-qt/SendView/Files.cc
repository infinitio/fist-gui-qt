#include <fist-gui-qt/SendView/Files.hh>
#include <fist-gui-qt/TwoStateIconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/globals.hh>

#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QDropEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QFile>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QUrl>

ELLE_LOG_COMPONENT("infinit.FIST.Files");

namespace fist
{
  namespace sendview
  {
    Files::Files(QWidget* parent)
      : Super(parent)
      , _foo(new QWidget(this))
      , _attach(new IconButton(QPixmap(":/send/files")))
      , _text(new QLabel(view::send::file_adder::text))
      , _expanser(
        new fist::TwoStateIconButton(QPixmap(":/send/show-files@2x"),
                                     QPixmap(":/send/hide-files@2x"),
                                     QPixmap(":/send/show-files@2x"),
                                     QPixmap(":/send/hide-files@2x"), true, 10))
      , _add_file(new IconButton(QPixmap(":/send/add-file")))
      , _files()
      , _separator(new HorizontalSeparator(this))
      , _list(new ListWidget(this))
      , _growing_area(new fist::gui::GrowingArea(_list, this))
      , _pulse_animation(new QPropertyAnimation(this, "pulseColor"))
    {
      this->_growing_area->expand();
      {
        this->_pulse_animation->setStartValue(view::send::file_adder::pulse);
        this->_pulse_animation->setEndValue(view::send::file_adder::background);
        this->_pulse_animation->setDuration(200);
      }
      this->setContentsMargins(0, 0, 0, 0);
      // Background.
      {
        QPalette palette = this->palette();
        {
          palette.setColor(QPalette::Window, view::send::file_adder::background);
        }
        this->setPalette(palette);
        this->setAutoFillBackground(true);
      }
      // Files
      {
        this->_list->setMaxRows(3);
        auto palette = this->_list->palette();
        palette.setColor(QPalette::Window, view::send::file_adder::background);
        this->_list->setPalette(palette);
        this->_list->setAutoFillBackground(true);
      }
      // Text.
      {
        view::send::file_adder::style(*this->_text);
      }

      auto* vlayout = new QVBoxLayout(this);
      vlayout->setSpacing(0);
      vlayout->setContentsMargins(0, 0, 0, 0);
      vlayout->setAlignment(Qt::AlignVCenter);
      vlayout->addWidget(new HorizontalSeparator(this));
      {
        // Create a widget in order to calculate the height and fix it.
        QWidget* adder = new QWidget(this);
        auto* layout = new QHBoxLayout(adder);
        layout->setAlignment(Qt::AlignVCenter);
        layout->setSpacing(6);
        layout->setContentsMargins(16, 13, 13, 13);
        layout->addWidget(this->_attach);
        layout->setSpacing(16);
        layout->addWidget(this->_text, 1);
        layout->addWidget(this->_expanser);
        layout->addWidget(this->_add_file);
        adder->adjustSize();
        adder->setFixedHeight(adder->size().height());
        vlayout->addWidget(adder);

        //vlayout->addLayout(layout);
      }

      vlayout->addWidget(this->_separator);
      vlayout->addWidget(this->_growing_area);
      this->adjustSize();

      this->_attach->installEventFilter(this);
      this->_text->installEventFilter(this);

      this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      this->setAcceptDrops(true);

      connect(this, SIGNAL(file_dropped(QUrl const&)),
              this, SLOT(add_file(QUrl const&)));

      connect(this->_expanser, SIGNAL(pressed()), this->_growing_area, SLOT(expand()));
      connect(this->_expanser, SIGNAL(released()), this->_growing_area, SLOT(shrink()));
      connect(this->_growing_area, SIGNAL(shrinked()), this, SLOT(update()));
      connect(this->_growing_area, SIGNAL(expanded()), this, SLOT(update()));

      connect(this, SIGNAL(file_added()), SLOT(_update_message()));
      connect(this, SIGNAL(file_removed()), SLOT(_update_message()));

      connect(this->_growing_area, SIGNAL(shrinking()), this->_separator, SLOT(hide()));
      connect(this->_growing_area, SIGNAL(expanding()), this->_separator, SLOT(show()));

      this->_expanser->hide();
      this->_separator->hide();
      this->repaint();
    }

    QSize
    Files::sizeHint() const
    {
      return QSize(320, Super::sizeHint().height());
    }

    QSize
    Files::minimumSizeHint() const
    {
      return this->sizeHint();
    }

    bool
    Files::eventFilter(QObject *obj, QEvent *event)
    {
      if (obj == this->_attach)
      {
        if (event->type() == QEvent::Enter || event->type() == QEvent::DragEnter)
        {
          this->_attach->set_pixmap(QPixmap(":/send/files-hover"));
        }
        else if (event->type() == QEvent::Leave || event->type() == QEvent::DragLeave || event->type() == QEvent::WindowBlocked)
        {
          this->_attach->set_pixmap(QPixmap(":/send/files"));
        }
      }

      return Super::eventFilter(obj, event);
    }

    void
    Files::_update_message()
    {
      if (this->_files.isEmpty())
        this->_text->setText("Add files");
      else
      {
        qint64 size = 0;
        for (auto const& file: this->_files.keys())
        {
          if (file.isLocalFile())
          {
            size += QFile(file.toLocalFile()).size();
          }
        }
        this->_text->setText(
          QString::fromStdString(
            elle::sprintf("%s file%s (%s)",
                          this->_files.size(), (this->_files.size() == 1) ? "" : "s",
                          readable_size(size))));
      }
    }

    void
    Files::clear()
    {
      this->_files.clear();
      this->_list->clearWidgets();
      this->_update_message();
      this->_expanser->hide();
      this->_separator->hide();
      this->repaint();
    }

    void
    Files::pulse()
    {
      this->_pulse_animation->stop();
      this->_pulse_animation->start();
    }

    void
    Files::setPulseColor(QColor const& color)
    {
      QPalette palette;
      palette.setColor(QPalette::Background, color);
      this->setAutoFillBackground(true);
      this->setPalette(palette);
    }

    void
    Files::on_entered()
    {
      ELLE_DEBUG_SCOPE("%s: mouse entered", *this);
      if (this->_files.isEmpty())
      {
        this->_attach->set_pixmap(QPixmap(":/send/files-hover"));
        view::send::file_adder::hover_style(*this->_text);
        this->setCursor(QCursor(Qt::PointingHandCursor));
      }
      else
      {
        this->_expanser->show();
      }
      this->repaint();
    }

    void
    Files::on_left()
    {
      ELLE_DEBUG_SCOPE("%s: mouse left", *this);
      if (this->_files.isEmpty())
      {
        this->_attach->set_pixmap(QPixmap(":/send/files"));
        view::send::file_adder::style(*this->_text);
        this->setCursor(QCursor(Qt::ArrowCursor));
      }
      else
      {
        if (this->_growing_area->state() == gui::GrowingArea::State::shrinking ||
            this->_growing_area->state() == gui::GrowingArea::State::shrinked)
          this->_expanser->hide();
      }
      this->repaint();
    }

    void
    Files::dropEvent(QDropEvent *event)
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
      view::send::file_adder::style(*this->_text);
    }

    void
    Files::add_file(QUrl const& path)
    {
      ELLE_TRACE_SCOPE("%s: add file: %s", *this, path);

      if (this->_files.contains(path))
        return;

      auto it = this->_files.insert(path, std::make_shared<FileItem>(path));
      emit file_added();
      connect(it->get(), SIGNAL(remove(QUrl const&)),
              this, SLOT(remove_file(QUrl const&)));
      this->_separator->show();
      this->_list->add_widget(this->_files[path]);
    }

    void
    Files::remove_file(QUrl const& path)
    {
      ELLE_TRACE_SCOPE("%s: remove file: %s", *this, path);

      auto it = this->_files.find(path);

      if (it != this->_files.end())
      {
        this->_list->remove_widget(it.value());
        this->_files.erase(it);
        emit file_removed();
      }
      else
      {
        ELLE_DEBUG_SCOPE("no file deleted");
        for (auto const& file: this->_files.keys())
          ELLE_DEBUG("%s", file);
      }

      if (this->_files.isEmpty())
      {
        this->_separator->hide();
        this->_expanser->hide();
      }
    }

    void
    Files::mouseReleaseEvent(QMouseEvent* event)
    {
      if (this->_files.isEmpty())
        emit this->clicked();
      else
      {
        this->_expanser->click();
      }
    }

    void
    Files::enterEvent(QEvent* event)
    {
      this->on_entered();
      Super::enterEvent(event);
    }

    void
    Files::leaveEvent(QEvent* event)
    {
      this->on_left();
      Super::leaveEvent(event);
    }

    void
    Files::print(std::ostream& stream) const
    {
      stream << "File adder";
    }
  }
}
