#ifndef FILEITEM_HH
# define FILEITEM_HH

# include <QGraphicsPixmapItem>
# include <QListWidgetItem>
# include <QFileIconProvider>
# include <QLabel>
# include <QString>

# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ListItem.hh>

class FileItem:
  public ListItem
{
public:
  FileItem(QString const& path);
  virtual ~FileItem() = default;

  virtual
  void
  trigger() override;
private:
  QLabel* _name;
  QLabel* _icon;
  IconButton* _remove;
};

#endif
