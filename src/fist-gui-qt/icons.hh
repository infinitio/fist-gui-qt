#ifndef FIST_GUI_QT_ICONS_HH
# define FIST_GUI_QT_ICONS_HH

# include <QPixmap>

# include <fist-gui-qt/gui/ImageSerie.hh>

namespace fist
{
  namespace icon
  {
    static const fist::gui::ImageSerie transferring_waiting_for_decistion{":/menu-bar/red%1", 18, 1};
    static const fist::gui::ImageSerie transferring{":/menu-bar/white%1", 18, 1};
    static const fist::gui::ImageSerie normal(":/menu-bar/white@2x");
    static const fist::gui::ImageSerie waiting_for_decistion(":/menu-bar/red@2x");
    static const fist::gui::ImageSerie inactive(":/menu-bar/grey@2x");
  }
}

#endif
