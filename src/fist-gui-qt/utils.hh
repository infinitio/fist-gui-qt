#ifndef UTILS_HH
# define UTILS_HH

# include <QUrl>
# include <QNetworkReply>

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

inline
std::ostream&
operator << (std::ostream& out, QRect const& r)
{
  out << "("
      << r.x() << ", "
      << r.y() << ", "
      << r.width() << ", "
      << r.height()
      << ")";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, QPoint const& p)
{
  out << "("
      << p.x() << ", "
      << p.y()
      << ")";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, QSize const& p)
{
  out << "("
      << p.width() << ", "
      << p.height()
      << ")";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, QString const& s)
{
  if (s.isNull())
    return out;
  return out << s.toStdString();
}

inline
std::ostream&
operator << (std::ostream& out, QUrl const& u)
{
  return out << "QUrl(" << u.toString() << ")";
}

inline
std::ostream&
operator << (std::ostream& out, QNetworkReply const& r)
{
  return out << "Request(" << r.url() << ")";
}

inline
QString
readable_size(qint64 size)
{
  int i = 0;
  std::vector<QString> units = {
    "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

  while (size > 1024)
  {
    size /= 1024.f;
    i++;
  }

  return QString("%1").arg(size) + " " + units[i];
};

#endif
