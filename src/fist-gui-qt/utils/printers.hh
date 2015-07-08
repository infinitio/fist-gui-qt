#ifndef FIST_GUI_QT_UTILS_PRINTERS_HH
# define FIST_GUI_QT_UTILS_PRINTERS_HH

# include <QRect>
# include <QPoint>
# include <QString>
# include <QUrl>
# include <QNetworkReply>
# include <QDateTime>

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
operator << (std::ostream& out, QString const& s)
{
  if (s.isNull())
    return out;
  return out << QString_to_utf8_string(s);
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
  while (size >= 1000)
  {
    size /= 1000.f;
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
pretty_date(QDateTime const& date,
            bool detailed = false)
{
  auto current = QDateTime::currentDateTime();
  auto local_date = date.toLocalTime();
  QString format =
    detailed
    ? "MMMM d - hh'h'mm'm'ss's'zz"
    : (local_date.secsTo(current) < seconds_since_midnight(current)
       ? "hh:mmA"
       : "MMM d");
  return local_date.toString(format);
}

inline
std::ostream&
operator << (std::ostream& out, QDateTime const& d)
{
  return out << "Date(" << pretty_date(d) << ")";
}

#endif
