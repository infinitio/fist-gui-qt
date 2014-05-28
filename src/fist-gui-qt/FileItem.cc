#include <QDir>
#include <QHBoxLayout>

#include <elle/log.hh>

#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/globals.hh>
#include <fist-gui-qt/utils.hh>

ELLE_LOG_COMPONENT("infinit.FIST.FileItem");

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
  this->_layout->setAlignment(Qt::AlignVCenter);
  ELLE_TRACE_SCOPE("%s: construction with path %s", *this, path);

  this->setContentsMargins(14, 0, 14, 0);
  this->_layout->setContentsMargins(0, 0, 0, 0);
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
