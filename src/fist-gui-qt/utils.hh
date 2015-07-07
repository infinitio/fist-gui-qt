#ifndef FIST_GUI_QT_UTILS_HH
# define FIST_GUI_QT_UTILS_HH

# include <QDateTime>
# include <QNetworkReply>
# include <QUrl>
# include <QRect>
# include <QPoint>

# define Q_PROPERTY_R(Type, Name, Read)                 \
  public:                                               \
    Type                                                \
    Read() const                                        \
    {                                                   \
      return this->_##Name;                             \
    }                                                   \
                                                        \
  private:                                              \
    Type _##Name;                                       \

# include <fist-gui-qt/utils/conversions.hh>
# include <fist-gui-qt/utils/printers.hh>

#endif
