#include <fist-gui-qt/SendView/Files.hh>
#include <fist-gui-qt/TwoStateIconButton.hh>
#include <fist-gui-qt/ListWidget.hh>
#include <fist-gui-qt/HorizontalSeparator.hh>
#include <fist-gui-qt/SendView/ui.hh>
#include <fist-gui-qt/SendView/FileAdder.hh>

#include <fist-gui-qt/utils.hh>

#include <elle/log.hh>

#include <QDropEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QDir>
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
      , _empty_adder(new EmptyFileAdder(this))
      , _non_empty_adder(new NonEmptyFileAdder(this))
      , _files()
      , _separator(new HorizontalSeparator(this))
      , _list(new ListWidget(
                this, ListWidget::Separator({QColor(0xF8, 0xF8, 0xF8)}, 10, 10),
                QColor{0xF8, 0xF8, 0xF8}))
      , _growing_area(new fist::gui::GrowingArea(_list, this))
    {
      this->setContentsMargins(0, 0, 0, 0);
      // Background.
      {
        QPalette palette = this->palette();
        {
          palette.setColor(QPalette::Window, view::file_adder::background);
        }
        this->setPalette(palette);
        this->setAutoFillBackground(true);
      }
      // Files
      {
        this->_list->setMaxRows(3);
        auto palette = this->_list->palette();
        palette.setColor(QPalette::Window, view::file_adder::background);
        this->_list->setPalette(palette);
        this->_list->setAutoFillBackground(true);
      }

      auto* vlayout = new QVBoxLayout(this);
      vlayout->setSpacing(0);
      vlayout->setContentsMargins(0, 0, 0, 0);
      vlayout->setAlignment(Qt::AlignVCenter);
      vlayout->addWidget(new HorizontalSeparator(this));
      vlayout->addWidget(this->_empty_adder);
      vlayout->addWidget(this->_non_empty_adder);
      vlayout->addWidget(this->_separator);
      vlayout->addWidget(this->_growing_area);
      this->adjustSize();

      this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

      this->_install_file_adder(this->_empty_adder);
      this->_install_file_adder(this->_non_empty_adder);
      connect(this->_non_empty_adder->expanser(), SIGNAL(pressed()), this->_growing_area, SLOT(expand()));
      connect(this->_non_empty_adder->expanser(), SIGNAL(released()), this->_growing_area, SLOT(shrink()));
      connect(this->_growing_area, SIGNAL(shrinked()), this, SLOT(update()));
      connect(this->_growing_area, SIGNAL(expanded()), this, SLOT(update()));

      connect(this, SIGNAL(file_added()), SLOT(_update_message()));
      connect(this, SIGNAL(file_removed()), SLOT(_update_message()));

      connect(this->_growing_area, SIGNAL(shrinking()), this->_separator, SLOT(hide()));
      connect(this->_growing_area, SIGNAL(expanding()), this->_separator, SLOT(show()));

      this->_empty_adder->show();
      this->_non_empty_adder->hide();
      this->_separator->hide();
      this->_growing_area->shrink();
      this->_growing_area->expand(true);
      this->repaint();
    }

    void
    Files::_install_file_adder(FileAdder* adder)
    {
      connect(adder, SIGNAL(file_dropped(QUrl const&)),
              this, SLOT(add_file(QUrl const&)));

      connect(adder, SIGNAL(clicked()), this, SIGNAL(clicked()));
      connect(adder, SIGNAL(dropped()), this, SIGNAL(dropped()));
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

    void
    Files::_update_message()
    {
      if (this->_files.isEmpty())
        this->_non_empty_adder->set_text("Add files");
      else
      {
        qint64 size = 0;
        for (auto const& file: this->_files.keys())
          size += this->_files[file]->size;
        this->_non_empty_adder->set_text(
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
      this->_empty_adder->show();
      this->_non_empty_adder->hide();
      this->_separator->hide();
      this->repaint();
    }

    void
    Files::add_file(QUrl const& file)
    {
      if (this->_files.contains(file))
        return;
      auto it = this->_files.insert(file, std::make_shared<FileItem>(file));
      connect(it->get(), SIGNAL(remove(QUrl const&)),
              this, SLOT(remove_file(QUrl const&)));
      this->_separator->show();
      this->_list->add_widget(this->_files[file]);
      if (this->_files.size() == 1)
      {
        this->_growing_area->shrink();
        this->_growing_area->expand(true);
      }
      this->_non_empty_adder->show();
      this->_empty_adder->hide();
      emit file_added();
    }

    void
    Files::add_files(QList<QUrl> const& paths)
    {
      ELLE_TRACE_SCOPE("%s: add files: %s", *this, paths);
      for (auto path: paths)
      {
        this->add_file(path);
      }
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
        this->_empty_adder->show();
        this->_non_empty_adder->hide();
        this->_separator->hide();
      }
    }

    void
    Files::print(std::ostream& stream) const
    {
      stream << "File adder";
    }
  }
}
