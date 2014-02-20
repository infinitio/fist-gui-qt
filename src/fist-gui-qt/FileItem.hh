#ifndef FILEITEM_HH
# define FILEITEM_HH

# include <QFileIconProvider>
# include <QGraphicsPixmapItem>
# include <QHBoxLayout>
# include <QLabel>
# include <QListWidgetItem>
# include <QString>
# include <QUrl>

# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ListItem.hh>

class FileItem:
  public ListItem
{
public:
  FileItem(QUrl const& path);

  virtual ~FileItem() = default;

  virtual
  void
  trigger() override;

Q_SIGNALS:
  void
  remove(QUrl const&);

private:
  QHBoxLayout* _layout;
  QUrl _path;
  QFile _file;
  QLabel* _name;
  QLabel* _icon;
  QLabel* _size;
  IconButton* _remove;

public:
  Q_OBJECT;
};

#endif
