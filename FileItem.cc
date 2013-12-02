#include <fist-gui-qt/FileItem.hh>

#include <QFormLayout>

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

FileItem::FileItem(QString const& path):
  ListItem(nullptr),
  _name(new QLabel(path)),
  _icon(new QLabel),
  _remove(new IconButton(QPixmap(":/icons/star.png"), // Remove.
                         true))
{
  auto* layout = new QFormLayout(this);
  QFileIconProvider icon_provider;
  this->_icon->setPixmap(icon_provider.icon(path).pixmap(18));
  layout->addRow(this->_icon, this->_name);
}

void
FileItem::trigger()
{
}
