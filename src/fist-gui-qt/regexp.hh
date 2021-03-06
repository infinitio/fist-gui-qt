#ifndef FIST_GUI_QT_LOGIN_REGEXP_HH
# define FIST_GUI_QT_LOGIN_REGEXP_HH

# include <QRegExp>
# include <QString>

namespace fist
{
  namespace regexp
  {
    namespace
    {
      static QString _email = "[a-z0-9\\._%+-]+@[a-z0-9\\.-]+\\.[a-z]{2,12}";
      static QString _uuid =
        "[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}";
    }
    namespace email
    {
      static QRegExp checker(_email, Qt::CaseInsensitive);
    }

    namespace uuid
    {
      static QRegExp checker(_uuid, Qt::CaseInsensitive);
    }
  }
}

#endif
