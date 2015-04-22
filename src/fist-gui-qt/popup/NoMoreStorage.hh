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

    private:
      void
      showEvent(QShowEvent* event) override;
    };
  }
}
#endif
