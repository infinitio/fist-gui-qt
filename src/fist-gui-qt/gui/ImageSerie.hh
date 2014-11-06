#ifndef FIST_GUI_QT_GUI_IMAGESERIE_HH
# define FIST_GUI_QT_GUI_IMAGESERIE_HH

# include <unordered_map>
# include <elle/attribute.hh>

# include <QString>
# include <QPixmap>

#include <elle/log.hh>

namespace fist
{
  namespace gui
  {
    struct ImageSerie
    {
      ImageSerie(QString const& pattern = "",
                 uint64_t number = 1,
                 uint64_t offset = 0);

      ImageSerie(ImageSerie const&) = default;

      // ImageSerie&
      // operator =(ImageSerie const&);

      // ImageSerie(QPixmap const& image);

      QPixmap const&
      next();

      ELLE_ATTRIBUTE_R(QString, pattern);
      ELLE_ATTRIBUTE_R(uint64_t, number);
      ELLE_ATTRIBUTE_R(uint64_t, offset);
      ELLE_ATTRIBUTE_R(uint64_t, index);
      typedef std::unordered_map<uint64_t, QPixmap> Images;
      ELLE_ATTRIBUTE_R(Images, images);
    };
  }
}

#endif
