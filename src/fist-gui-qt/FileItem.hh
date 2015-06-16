#ifndef FIST_GUI_QT_FILEITEM_HH
# define FIST_GUI_QT_FILEITEM_HH

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
  typedef ListItem Super;
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
public:
  quint64 size;
private:
  QLabel* _size;
  IconButton* _remove;


public:
  Q_OBJECT;
};

#endif
