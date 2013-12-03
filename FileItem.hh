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
  QSize
  sizeHint() const override;

  virtual
  void
  trigger() override;

Q_SIGNALS:
  void
  remove(QString const&);

private:
  QFile _file;
  QLabel* _name;
  QLabel* _icon;
  QLabel* _size;
  IconButton* _remove;

public:
  Q_OBJECT;
};

#endif
