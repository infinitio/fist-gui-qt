#ifndef FIST_GUI_QT_GUI_ICONPROVIDER_KNOWN_HH
# define FIST_GUI_QT_GUI_ICONPROVIDER_KNOWN_HH

# include <fist-gui-qt/gui/IconProvider.hh>

namespace fist
{
  namespace gui
  {
    static
    const
    IconProvider::TypeTranslator known_types{
      { "323", IconProvider::Type::document },
      { "7z", IconProvider::Type::archive },
      { "ai", IconProvider::Type::illustrator },
      { "aif", IconProvider::Type::music },
      { "aifc", IconProvider::Type::music },
      { "aiff", IconProvider::Type::music },
      { "asf", IconProvider::Type::video },
      { "asr", IconProvider::Type::video },
      { "asx", IconProvider::Type::video },
      { "au", IconProvider::Type::music },
      { "avi", IconProvider::Type::video },
      { "bas", IconProvider::Type::document },
      { "bmp", IconProvider::Type::image },
      { "c", IconProvider::Type::text },
      { "css", IconProvider::Type::text },
      { "doc", IconProvider::Type::text },
      { "dot", IconProvider::Type::text },
      { "eps", IconProvider::Type::illustrator },
      { "etx", IconProvider::Type::text },
      { "gif", IconProvider::Type::cat },
      { "gtar", IconProvider::Type::archive },
      { "gz", IconProvider::Type::archive },
      { "h", IconProvider::Type::text },
      { "htc", IconProvider::Type::text },
      { "htm", IconProvider::Type::text },
      { "html", IconProvider::Type::text },
      { "htt", IconProvider::Type::text },
      { "ico", IconProvider::Type::image },
      { "ief", IconProvider::Type::image },
      { "jfif", IconProvider::Type::image },
      { "jpe", IconProvider::Type::image },
      { "jpeg", IconProvider::Type::image },
      { "jpg", IconProvider::Type::image },
      { "latex", IconProvider::Type::text },
      { "lsf", IconProvider::Type::video },
      { "lsx", IconProvider::Type::video },
      { "m3u", IconProvider::Type::music },
      { "mid", IconProvider::Type::music },
      { "mov", IconProvider::Type::video },
      { "movie", IconProvider::Type::video },
      { "mp2", IconProvider::Type::video },
      { "mp3", IconProvider::Type::music },
      { "mp4", IconProvider::Type::video },
      { "mpa", IconProvider::Type::video },
      { "mpe", IconProvider::Type::video },
      { "mpeg", IconProvider::Type::video },
      { "mpg", IconProvider::Type::video },
      { "mpv2", IconProvider::Type::video },
      { "pbm", IconProvider::Type::image },
      { "pgm", IconProvider::Type::image },
      { "pot", IconProvider::Type::powerpoint },
      { "png", IconProvider::Type::image },
      { "ppm", IconProvider::Type::image },
      { "pps", IconProvider::Type::powerpoint },
      { "ppt", IconProvider::Type::powerpoint },
      { "pub", IconProvider::Type::illustrator },
      { "qt", IconProvider::Type::video },
      { "ra", IconProvider::Type::music },
      { "ram", IconProvider::Type::music },
      { "ras", IconProvider::Type::image },
      { "rgb", IconProvider::Type::image },
      { "rmi", IconProvider::Type::music },
      { "rtf", IconProvider::Type::text },
      { "rtx", IconProvider::Type::text },
      { "sct", IconProvider::Type::text },
      { "sh", IconProvider::Type::text },
      { "snd", IconProvider::Type::music },
      { "stm", IconProvider::Type::text },
      { "svg", IconProvider::Type::image },
      { "tar", IconProvider::Type::archive },
      { "tgz", IconProvider::Type::archive },
      { "tif", IconProvider::Type::image },
      { "tiff", IconProvider::Type::image },
      { "tsv", IconProvider::Type::document },
      { "txt", IconProvider::Type::document },
      { "uls", IconProvider::Type::document },
      { "vcf", IconProvider::Type::document },
      { "wav", IconProvider::Type::music },
      { "wcm", IconProvider::Type::document },
      { "wdb", IconProvider::Type::document },
      { "wks", IconProvider::Type::document },
      { "wps", IconProvider::Type::document },
      { "wri", IconProvider::Type::document },
      { "xbm", IconProvider::Type::image },
      { "xla", IconProvider::Type::document },
      { "xlc", IconProvider::Type::document },
      { "xlm", IconProvider::Type::document },
      { "xls", IconProvider::Type::document },
      { "xlt", IconProvider::Type::document },
      { "xlw", IconProvider::Type::document },
      { "xpm", IconProvider::Type::image },
      { "xwd", IconProvider::Type::image },
      { "z", IconProvider::Type::archive },
      { "zip", IconProvider::Type::archive },
      { "psd", IconProvider::Type::photoshop },
      { "xcf", IconProvider::Type::photoshop },
    };

    static
    IconProvider::TypeIcon const& known_icons()
    {
      static std::unique_ptr<IconProvider::TypeIcon> known;
      if (known == nullptr)
      {
        known.reset(new IconProvider::TypeIcon{
          { IconProvider::Type::archive, QPixmap(":/mimetypes/archive") },
          { IconProvider::Type::cat, QPixmap(":/mimetypes/cat") },
          { IconProvider::Type::document, QPixmap(":/mimetypes/document") },
          { IconProvider::Type::folder, QPixmap(":/mimetypes/folder") },
          { IconProvider::Type::illustrator, QPixmap(":/mimetypes/illustrator") },
          { IconProvider::Type::music, QPixmap(":/mimetypes/music") },
          { IconProvider::Type::photoshop, QPixmap(":/mimetypes/photoshop") },
          { IconProvider::Type::image, QPixmap(":/mimetypes/picture") },
          { IconProvider::Type::powerpoint, QPixmap(":/mimetypes/powerpoint") },
          { IconProvider::Type::text, QPixmap(":/mimetypes/document") },
          { IconProvider::Type::unknown, QPixmap(":/mimetypes/document") },
          { IconProvider::Type::video, QPixmap(":/mimetypes/video") },
        });
      }
      return *known;
    }
  }
}
#endif
