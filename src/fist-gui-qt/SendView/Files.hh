#ifndef ADDFILEITEM_HH
# define ADDFILEITEM_HH

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

    public:
      Q_PROPERTY(QColor pulseColor
                 READ pulseColor
                 WRITE setPulseColor);
      Q_PROPERTY_R(QColor, pulse_color, pulseColor);

      void
      setPulseColor(QColor const& color);

    public slots:
      void
      on_entered();

      void
      on_left();

      void
      dropEvent(QDropEvent*) override;

    signals:
      void
      clicked();

      void
      file_dropped(QUrl const&);

      void
      dropped();

    public slots:
      void
      add_file(QUrl const& file);

      void
      add_files(QList<QUrl> const& path);

      void
      remove_file(QUrl const& path);

    public slots:
      void
      pulse();

      void
      clear();

    private slots:
      void
      _update_message();

    private:
      void
      mouseReleaseEvent(QMouseEvent* event) override;

      void
      enterEvent(QEvent* event) override;

      void
      leaveEvent(QEvent* event) override;

    private:
      bool
      eventFilter(QObject *obj, QEvent *event) override;

    signals:
      void
      file_added();

      void
      file_removed();
    private:
      ELLE_ATTRIBUTE(QWidget*, foo);
      ELLE_ATTRIBUTE_R(IconButton*, attach);
      ELLE_ATTRIBUTE_R(QLabel*, text);
      ELLE_ATTRIBUTE_R(fist::TwoStateIconButton*, expanser);
      ELLE_ATTRIBUTE_R(IconButton*, add_file);
      typedef QHash<QUrl, std::shared_ptr<ListItem>> List;
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
