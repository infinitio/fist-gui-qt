#ifndef FIST_GUI_QT_SEND_VIEW_FILEADDER_HH
# define FIST_GUI_QT_SEND_VIEW_FILEADDER_HH

# include <QWidget>

# include <elle/attribute.hh>

# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/TwoStateIconButton.hh>
# include <fist-gui-qt/IconButton.hh>

namespace fist
{
  namespace sendview
  {

    class FileAdder
      : public QWidget
    {
      typedef QWidget Super;

    public:
      FileAdder(QWidget* parent = nullptr);

      void
      dropEvent(QDropEvent*) override;

    signals:
      void
      clicked();

      void
      file_dropped(QUrl const&);

      void
      dropped();

    protected:
      void
      paintEvent(QPaintEvent*) override;

    private:
      Q_OBJECT;
    };

    class EmptyFileAdder
     : public FileAdder
    {
    public:
      EmptyFileAdder(QWidget* parent = nullptr);

    private:
      void
      dragEnterEvent(QDragEnterEvent * event) override;

      void
      enterEvent(QEvent * event) override;

      void
      dragLeaveEvent(QDragLeaveEvent * event) override;

      void
      leaveEvent(QEvent* event) override;

      void
      mouseReleaseEvent(QMouseEvent* event) override;

    public:
      ELLE_ATTRIBUTE_R(QLabel*, icon);
      ELLE_ATTRIBUTE_R(QLabel*, title);
      ELLE_ATTRIBUTE_R(QLabel*, subtitle);

      ELLE_ATTRIBUTE_Rw(bool, hover);
    private:
      void
      paintEvent(QPaintEvent*) override;
    };

    class NonEmptyFileAdder
      : public FileAdder
    {
    public:
      NonEmptyFileAdder(QWidget* parent = nullptr);

    private:
      bool
      eventFilter(QObject *obj, QEvent *event) override;

    public:
      void
      set_text(QString const& text,
               bool save = true);
    private:
      // Adder.
      ELLE_ATTRIBUTE_R(IconButton*, attach);
      ELLE_ATTRIBUTE_R(QLabel*, message);
      ELLE_ATTRIBUTE_RX(fist::TwoStateIconButton*, expanser);
      ELLE_ATTRIBUTE_R(QPushButton*, add);

      ELLE_ATTRIBUTE(QString, old_text);
    };

  }
}


#endif
