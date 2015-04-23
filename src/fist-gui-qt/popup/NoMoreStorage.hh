#ifndef FIST_GUI_QT_SENDVIEW_NOMORESTORAGEWINDOW_HH
# define FIST_GUI_QT_SENDVIEW_NOMORESTORAGEWINDOW_HH

# include <QMainWindow>

# include <elle/attribute.hh>

namespace fist
{
  namespace popup
  {
    class NoMoreStorage
      : public QMainWindow
    {
      typedef QMainWindow Super;
    public:
      NoMoreStorage(QWidget* parent);

    private slots:
      void
      _go_to_website();

    private:
      void
      showEvent(QShowEvent* event) override;

    private:
      Q_OBJECT;
    };
  }
}
#endif
