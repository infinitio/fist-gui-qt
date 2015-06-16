#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QWebView>

#include <elle/log.hh>

#include <fist-gui-qt/login/facebook/ConnectWindow.hh>

ELLE_LOG_COMPONENT("FIST.login.FacebookConnect");

#define REDIRECT_URI "https://www.facebook.com/connect/login_success.html"

QT_BEGIN_NAMESPACE
QDataStream&
operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
  stream << JAR_VERSION;
  stream << quint32(list.size());
  for (int i = 0; i < list.size(); ++i)
    stream << list.at(i).toRawForm();
  return stream;
}

QDataStream&
operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
  list.clear();

  quint32 version;
  stream >> version;

  if (version != JAR_VERSION)
    return stream;

  quint32 count;
  stream >> count;
  for(quint32 i = 0; i < count; ++i)
  {
    QByteArray value;
    stream >> value;
    QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
    for (int j = 0; j < newCookies.count(); ++j)
      list.append(newCookies.at(j));
    if (stream.atEnd())
      break;
  }
  return stream;
}
QT_END_NAMESPACE

namespace fist
{
  namespace login
  {
    namespace facebook
    {
      ConnectWindow::Cookies::Cookies(QObject* parent)
        : QNetworkCookieJar(parent)
      {
        qRegisterMetaTypeStreamOperators<QList<QNetworkCookie>>("QList<QNetworkCookie>");
        this->load();
      }

      void
      ConnectWindow::Cookies::load()
      {
        auto cookies = fist::settings()["cookies"].get("list");
        this->setAllCookies(qvariant_cast<QList<QNetworkCookie>>(cookies));
      }

      void
      ConnectWindow::Cookies::clear()
      {
        fist::settings()["cookies"].remove("list");
      }

      void
      ConnectWindow::Cookies::save()
      {
        ELLE_TRACE_SCOPE("get cookies");
        QList<QNetworkCookie> cookies = this->allCookies();
        ELLE_DEBUG("number of cookies: %s", cookies.size());
        for (int i = cookies.count() - 1; i >= 0; --i)
        {
          auto& cookie = cookies.at(i);
          ELLE_DEBUG("%s -> %s -> %s", cookie.domain(), QString(cookie.value()), QString(cookie.toRawForm()));
          if (cookies.at(i).isSessionCookie())
            cookies.removeAt(i);
        }
        ELLE_DEBUG("number of cookies: %s", cookies.size());
        auto variant_cookies = QVariant::fromValue<QList<QNetworkCookie>>(cookies);
        ELLE_DEBUG("is null ? %s", variant_cookies.isNull());
        fist::settings()["cookies"].set("list", variant_cookies);
      }

      ConnectWindow::ConnectWindow(QString const& app_id,
                                   QWidget* parent,
                                   bool load)
        : QMainWindow(parent)
        , _app_id(app_id)
        , _token()
        , _cookies{new Cookies{this}}
      {
        auto* web_view = new QWebView(this);
        this->setCentralWidget(web_view);
        auto* network_access_manager = web_view->page()->networkAccessManager();
        network_access_manager->setCookieJar(this->_cookies);
        if (load)
        {
          connect(network_access_manager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(_handle_reply(QNetworkReply*)));
          web_view->load(
            QUrl(
              QString::fromStdString(
                elle::sprintf("https://www.facebook.com/dialog/oauth?"
                              "client_id=%s"
                              "&redirect_uri=" REDIRECT_URI
                              "&response_type=token"
                              "&scope=email,public_profile,user_friends"
                              "&display=popup",
                              this->_app_id.toStdString()
                  ))));
        }
      }

      void
      ConnectWindow::_handle_reply(QNetworkReply* reply)
      {
        QUrl requested_url = reply->request().url();
        ELLE_DEBUG("requested url: %s", requested_url);
        this->_cookies->save();
        if (requested_url.toString().startsWith(REDIRECT_URI))
        {
          // Facebook returns an url like:
          // REDIRECT_URI#access_token=<access_token>&expire=4500
          // the replace is a 'cheat' to easily extract the access_token.
          requested_url = QUrl{requested_url.toString().replace("#", "?")};
          if (requested_url.hasQueryItem("access_token"))
          {
            auto access_token =
              requested_url.allQueryItemValues("access_token")[0];
            this->_token = access_token;
            emit success(access_token);
          }
          else
          {
            if (requested_url.hasQueryItem("error"))
              ELLE_WARN("failure: %s",
                        requested_url.allQueryItemValues("error")[0])
                emit failure(requested_url.allQueryItemValues("error")[0]);
            else
              emit failure("unknown error");
          }
          this->hide();
        }
        reply->deleteLater();
      }

      void
      ConnectWindow::closeEvent(QCloseEvent *event)
      {
        ELLE_TRACE_SCOPE("close window");
        emit failure("");
        QMainWindow::closeEvent(event);
      }
    }
  }
}
