#include <fist-gui-qt/FileItem.hh>

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

QSize
FileItem::sizeHint() const
{
  return QSize(this->width(), 24);
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
  ListItem(nullptr),
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
  // Toggle word wrap to filename.
  this->_name->setWordWrap(true);

  // Make size grey.
  {
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(150, 150, 150));
    this->_size->setPalette(palette);
  }

  auto* layout = new QHBoxLayout(this);
  QFileIconProvider icon_provider;
  this->_icon->setPixmap(icon_provider.icon(_file).pixmap(18));
  layout->addWidget(this->_icon);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Minimum));
  layout->addWidget(this->_name);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Expanding, QSizePolicy::Minimum));
  layout->addWidget(this->_size);
  layout->addItem(new QSpacerItem(0, 0,
                                  QSizePolicy::Minimum, QSizePolicy::Minimum));
  layout->addWidget(this->_remove);

  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
FileItem::trigger()
{
}
