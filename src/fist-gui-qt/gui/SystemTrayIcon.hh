#ifndef FIST_GUI_QT_GUI_SYSTEMTRAYICON_HH
# define FIST_GUI_QT_GUI_SYSTEMTRAYICON_HH

# include <QTimer>
# include <QObject>
# include <QSystemTrayIcon>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <fist-gui-qt/gui/ImageSerie.hh>

namespace fist
{
  namespace gui
  {
    namespace systray
    {
      class Icon
        : public QObject
      {
      public:
        Icon(ImageSerie const& serie,
             QObject* parent = nullptr);

        void
        show();

        void
        hide();

      private slots:
        void
        _change_icon();

      public:
        void
        set_icon(QString const& image);

        void
        set_icon(fist::gui::ImageSerie const& serie);

        QSystemTrayIcon*
        inner();

        ELLE_ATTRIBUTE(QSystemTrayIcon*, icon);
        ELLE_ATTRIBUTE(QTimer*, rotation_timer);
        ELLE_ATTRIBUTE(ImageSerie, serie);

      private:
        Q_OBJECT;
      };
    }
  }
}

#endif
