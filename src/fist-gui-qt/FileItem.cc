#include <fist-gui-qt/FileItem.hh>
#include <fist-gui-qt/globals.hh>

#include <QHBoxLayout>

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
  int i = -1;
  std::vector<QString> units = {"kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
  do
  {
    size /= 1024.f;
    i++;
  }
  while (size > 1024);

  return QString("%1").arg(size) + " " + units[i];
};

FileItem::FileItem(QString const& path):
  ListItem(nullptr, view::send::file::background, false),
  _layout(new QHBoxLayout(this)),
  _file(path),
  _name(new QLabel(_file.fileName())),
  _icon(new QLabel),
  _size(new QLabel(readable_size(_file.size()))),
  _remove(new IconButton(QPixmap(":/icons/delete.png"), // Remove.
                         false,
                         [this]
                         {
                           emit remove(this->_file.fileName());
                         }))
{
  this->setContentsMargins(6, 0, 6, 0);

  // Name.
  {
    view::send::file::name::style(*this->_name);
  }

  // Size.
  {
    view::send::file::size::style(*this->_size);
  }

  QFileIconProvider icon_provider;
  this->_icon->setPixmap(icon_provider.icon(_file).pixmap(18));
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
