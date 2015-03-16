#ifndef FIST_GUI_QT_LOGIN_FACEBOOKCONNECTWINDOW_HH
# define FIST_GUI_QT_LOGIN_FACEBOOKCONNECTWINDOW_HH

# include <QMainWindow>
# include <QNetworkReply>
# include <QNetworkCookieJar>

# include <elle/log.hh>

# include <fist-gui-qt/Settings.hh>
# include <fist-gui-qt/utils.hh>

#define JAR_VERSION 1

QT_BEGIN_NAMESPACE
QDataStream&
operator<<(QDataStream &stream, const QList<QNetworkCookie> &list);
QDataStream&
operator>>(QDataStream &stream, QList<QNetworkCookie> &list);
QT_END_NAMESPACE

namespace fist
{
  namespace login
  {
    namespace facebook
    {
      class ConnectWindow
        : public QMainWindow
      {
        class Cookies
          : public QNetworkCookieJar
        {
        public:
          Cookies(QObject* parent = nullptr);

          void
          clear();

          void
          load();

          void
          save();
        };

      private:
        void
        closeEvent(QCloseEvent *event) override;
      public:
        ConnectWindow(QString const& app_id,
                      QWidget* parent = nullptr,
                      bool load = true);
      private slots:
        void
        _handle_reply(QNetworkReply* reply);

      signals:
        void
        success(QString const& code);

        void
        failure(QString const& error);

      private:
        ELLE_ATTRIBUTE(QString, app_id);
        ELLE_ATTRIBUTE_X(Cookies*, cookies);
      private:
        Q_OBJECT;
      };
    }
  }
}

#endif
