#ifndef FIST_GUI_QT_SEND_VIEW_ADDFILEITEM_HH
# define FIST_GUI_QT_SEND_VIEW_ADDFILEITEM_HH

# include <memory>

# include <QColor>
# include <QHash>
# include <QLabel>
# include <QList>
# include <QPropertyAnimation>
# include <QUrl>
# include <QWidget>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <fist-gui-qt/FileItem.hh>
# include <fist-gui-qt/IconButton.hh>
# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>
# include <fist-gui-qt/gui/GrowingArea.hh>
# include <fist-gui-qt/utils.hh>

namespace fist
{
  namespace sendview
  {

    class Files:
      public QWidget,
      public elle::Printable
    {
      typedef QWidget Super;

    public:
      Files(QWidget* parent = nullptr);

      virtual
      QSize
      sizeHint() const override;

      virtual
      QSize
      minimumSizeHint() const override;

      // int
      // widthHint() const override;

    public slots:
      void
      add_file(QUrl const& file);

      void
      add_files(QList<QUrl> const& path);

      void
      remove_file(QUrl const& path);
    private:
      void
      _install_file_adder(FileAdder* adder);

    public slots:
      void
      clear();

    private slots:
      void
      _update_message();

    signals:
      void
      file_added();

      void
      file_removed();

      void
      clicked();

      void
      dropped();

    private:
      typedef QHash<QUrl, std::shared_ptr<FileItem>> List;
      ELLE_ATTRIBUTE_RX(EmptyFileAdder*, empty_adder);
      ELLE_ATTRIBUTE_RX(NonEmptyFileAdder*, non_empty_adder);
      ELLE_ATTRIBUTE_R(List, files);
      ELLE_ATTRIBUTE(HorizontalSeparator*, separator);
      ELLE_ATTRIBUTE_R(ListWidget*, list);
      ELLE_ATTRIBUTE_R(fist::gui::GrowingArea*, growing_area);
      ELLE_ATTRIBUTE(QPropertyAnimation*, pulse_animation);
    private:
      Q_OBJECT;

/*----------.
  | Printable |
  `----------*/
      void
      print(std::ostream& stream) const override;
    };
  }
}

#endif
