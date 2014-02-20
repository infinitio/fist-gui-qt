#include <QDir>
#include <QHBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/globals.hh>


ELLE_LOG_COMPONENT("infinit.FIST.FileItem");

namespace
{
  struct Separator:
    public QFrame
  {
    Separator()
    {
      this->setFrameShape(QFrame::VLine);
      this->setFrameShadow(QFrame::Sunken);
    }
  };
}

static
QString
readable_size(qint64 size)
{
  int i = 0;
  std::vector<QString> units = {
    "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

  while (size > 1024)
  {
    size /= 1024.f;
    i++;
  }

  return QString("%1").arg(size) + " " + units[i];
};

FileItem::FileItem(QUrl const& path):
  ListItem(nullptr, view::send::file::background, false),
  _layout(new QHBoxLayout(this)),
  _path(path),
  _file(path.toLocalFile()),
  _name(new QLabel(QDir::toNativeSeparators(path.toLocalFile()).split(QDir::separator()).last())),
  _icon(new QLabel),
  _size(new QLabel(readable_size(this->_file.size()))),
  _remove(new IconButton(QPixmap(":/icons/delete.png"), // Remove.
                         false,
                         [this]
                         {
                           emit remove(this->_path);
                         }))
{
  ELLE_TRACE_SCOPE("%s: construction with path %s", *this, path);

  this->setContentsMargins(6, 0, 6, 0);

  // Name.
  {
    view::send::file::name::style(*this->_name);
    this->_name->setToolTip(this->_name->text());
  }

  // Size.
  {
    view::send::file::size::style(*this->_size);
  }

  QFileIconProvider icon_provider;
  if (QFileInfo(this->_file).isDir())
    this->_icon->setPixmap(icon_provider.icon(QFileIconProvider::Folder).pixmap(18));
  else
    this->_icon->setPixmap(icon_provider.icon(this->_file).pixmap(18));

  this->_layout->addWidget(this->_icon);
  this->_layout->addItem(new QSpacerItem(4, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Minimum));
  this->_layout->addWidget(this->_name);
  this->_layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Expanding, QSizePolicy::Minimum));
  this->_layout->addWidget(this->_size);
  this->_layout->addItem(new QSpacerItem(4, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Minimum));
  this->_layout->addWidget(this->_remove);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  this->setFixedHeight(42);
  this->adjustSize();
}

void
FileItem::trigger()
{
}
