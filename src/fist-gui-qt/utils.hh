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

static
int
seconds_since_midnight(QDateTime const& date)
{
  auto time = date.time();
  return time.hour() * 3600 + time.minute() * 60 + time.second();
}

inline
QString
pretty_date(QDateTime const& date)
{
  auto current = QDateTime::currentDateTime();
  auto local_date = date.toLocalTime();
  QString format(local_date.secsTo(current) < seconds_since_midnight(current)
                 ? "h:mA"
                 : "MMM d");
  return local_date.toString(format);
}

inline
std::string
QString_to_utf8_string(QString const& input)
{
  auto array = input.toUtf8();
  return std::string{array.constData()};
}

#endif
