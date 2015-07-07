#ifndef FIST_GUI_QT_UTILS_CONVERSIONS_HH
# define FIST_GUI_QT_UTILS_CONVERSIONS_HH

inline
std::string
QString_to_utf8_string(QString const& input)
{
  auto array = input.toUtf8();
  return std::string{array.constData()};
}

inline
QString
QString_from_utf8_string(std::string const& input)
{
  return QString::fromUtf8(input.c_str());
}

inline
QString
url_encode(QString const& input)
{
  return QString(QUrl::toPercentEncoding(input)).replace("+", "%20");
}

#endif
