#include <QDir>
#include <QHBoxLayout>
#include <QtCore>

#include <elle/log.hh>

#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/SendView/ui.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.FileItem");

static
quint64
compute_size(QFileInfo const& info)
{
  quint64 sizex = 0;
  if (info.isDir())
  {
    QDir dir{info.filePath()};
    QFileInfoList list = dir.entryInfoList(
      QDir::Files | QDir::Dirs |  QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QtConcurrent::blockingFilter(
      list,
      [&sizex] (QFileInfo const& fileInfo) -> bool
      {
        if(fileInfo.isDir())
          sizex += compute_size(fileInfo);
        else
          sizex += fileInfo.size();
        return false;
      });
  }
  if (info.isFile())
  {
    sizex += info.size();
  }
  return sizex;
}

FileItem::FileItem(QUrl const& path):
  ListItem(nullptr, fist::sendview::view::file::background, false),
  _layout(new QHBoxLayout(this)),
  _path(path),
  _file(path.toLocalFile()),
  _name(new QLabel(QDir::toNativeSeparators(path.toLocalFile()).split(QDir::separator()).last(), this)),
  _icon(new QLabel(this)),
  size(compute_size(QFileInfo(path.toLocalFile()))),
  _size(new QLabel(readable_size(this->size), this)),
  _remove(new IconButton(":/send/delete", // Remove.
                         this,
                         QSize(),
                         [this]
                         {
                           emit remove(this->_path);
                         }))
{
  this->_layout->setAlignment(Qt::AlignVCenter);
  ELLE_TRACE_SCOPE("%s: construction with path %s", *this, path);

  this->setContentsMargins(14, 0, 14, 0);
  this->_layout->setContentsMargins(0, 0, 0, 0);
  // Name.
  {
    fist::sendview::view::file::name::style(*this->_name);
    this->_name->setToolTip(this->_name->text());
  }

// Size.
  {
    fist::sendview::view::file::size::style(*this->_size);
  }

  QFileIconProvider icon_provider;
  if (QFileInfo(this->_file).isDir())
    this->_icon->setPixmap(icon_provider.icon(QFileIconProvider::Folder).pixmap(18));
  else
    this->_icon->setPixmap(icon_provider.icon(this->_file).pixmap(18));

  this->_layout->addWidget(this->_icon);
  this->_layout->addSpacing(10);
  this->_layout->addWidget(this->_name);
  this->_layout->addStretch();
  this->_layout->addWidget(this->_size);
  this->_layout->addSpacing(4);
  this->_layout->addWidget(this->_remove);

  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  this->setFixedHeight(42);
  this->adjustSize();
}

void
FileItem::trigger()
{
}
