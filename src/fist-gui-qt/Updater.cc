#include <QApplication>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QLabel>
#include <QMap>
#include <QMovie>
#include <QNetworkRequest>
#include <QPixmap>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/assert.hh>

#include <common/common.hh>

#include <version.hh>

#include <fist-gui-qt/Updater.hh>
#include <fist-gui-qt/utils.hh>

#ifdef INFINIT_WINDOWS
# include <stringapiset.h>
# include <shellapi.h>
#endif

ELLE_LOG_COMPONENT("infinit.FIST.Updater");

Updater::Updater(QUrl const& version_url,
                 QObject* parent):
  QObject(parent),
  _version_url(version_url),
  _updater_url(),
  _installer(
    new QFile(
      QDir::toNativeSeparators(
        QString::fromStdString(common::infinit::home()) + QDir::separator() +
#ifdef INFINIT_WINDOWS
                 "installer.exe"
#else
                 "installer"
#endif
                 ))),
  _loading_dialog(new LoadingDialog(this)),
  _network_manager(new QNetworkAccessManager(this))
{
  connect(this, SIGNAL(update_available(bool)),
          this->_loading_dialog, SLOT(accept_reject_mode(bool)));
  connect(this, SIGNAL(updating()),
          this->_loading_dialog, SLOT(loading_mode()));
  connect(this, SIGNAL(no_update_available()),
          this, SLOT(_close_dialog()));
  connect(this->_loading_dialog->accept_button(), SIGNAL(released()),
          this, SLOT(update()));
  connect(this->_loading_dialog->reject_button(), SIGNAL(released()),
          this, SLOT(_close_dialog()));
  connect(this, SIGNAL(update_error(QString const&)),
          this, SLOT(_on_error(QString const&)));
  connect(this->_network_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(_handle_reply(QNetworkReply*)));

  // Delete the old installer file.
  if (this->_installer->exists())
  {
    ELLE_TRACE("%s: an old installer was present at %s: deleting it.",
               *this, this->_installer->fileName());
    this->_installer->remove();
  }
}

void
Updater::update()
{
  ELLE_TRACE_SCOPE("%s: update application", *this);
  this->_loading_dialog->text("Downloading update");

  emit this->updating();

  QNetworkRequest request;
  request.setUrl(this->_updater_url);
  request.setRawHeader(
    "User-Agent",
    QString::fromStdString(elle::sprintf("Windows %s", INFINIT_VERSION)).toUtf8());

  this->_reply.reset(this->_network_manager->get(request));
}

void
Updater::check_for_updates()
{
  ELLE_TRACE_SCOPE("%s: check for updates at %s", *this, this->_version_url);

  QNetworkRequest request;
  request.setUrl(this->_version_url);
  request.setRawHeader(
    "User-Agent",
    QString::fromStdString(elle::sprintf("Windows %s", INFINIT_VERSION)).toUtf8());

  this->_reply.reset(this->_network_manager->get(request));

  this->_loading_dialog->setModal(true);
  this->_loading_dialog->show();
  // this->_loading_dialog->exec();
}

void
Updater::_handle_reply(QNetworkReply* reply)
{
  ELLE_ASSERT(reply != nullptr);
  ELLE_TRACE_SCOPE("%s: reply from %s", *this, *reply);

  if (this->_reply->error() != QNetworkReply::NoError)
  {
    ELLE_WARN("something went wrong: %s", this->_reply->error());

    emit update_error(reply->errorString());

    if (this->_reply->url() == this->_updater_url)
    {
      if (this->_installer->exists())
      {
        ELLE_WARN("the updater was partialy downloaded...");
        this->_installer->remove();
      }
      else
      {
        ELLE_WARN("unable to check for udpates");
      }
    }
  }
  else if (this->_reply->isRunning())
  {
    ELLE_DUMP("is running...");
  }
  else if (this->_reply->isFinished())
  {
    if (this->_reply->url() == this->_updater_url)
    {
      this->_update(reply);
    }
    else if (this->_reply->url() == this->_version_url)
    {
      this->_check_if_up_to_date(reply);
    }
  }
}

void
Updater::_check_if_up_to_date(QNetworkReply* reply)
{
  ELLE_TRACE_SCOPE("%s: check for new update", *this);

  QXmlStreamReader xr(reply->readAll());
  QMap<QString,QString> updater_info;

  ELLE_TRACE_SCOPE("start reading");

  {
    QXmlStreamReader::TokenType token = xr.readNext();

    if(token == QXmlStreamReader::StartDocument)
    {
      ELLE_DEBUG("first token, ignored");
    }

    xr.readNext();

    if (xr.name().toString() != "updater")
      ELLE_ERR("xml file not valide");
  }

  while(!xr.atEnd() && !xr.hasError())
  {
    ELLE_TRACE_SCOPE("read a new element");
    // Read the next element.
    QXmlStreamReader::TokenType token = xr.readNext();

    if(token == QXmlStreamReader::StartElement)
    {
      auto name = xr.name().toString();

      ELLE_DEBUG_SCOPE("%s:", name);
      xr.readNext();

      if (name == "rich_description")
      {
        while (!(xr.tokenType() == QXmlStreamReader::EndElement && xr.name() == "rich_description"))
        {
          elle::SafeFinally next([&] { xr.readNext(); });
          updater_info[name].append(xr.text().toString());
        }
      }
      else
      {
        // If it's not data, continue.
        if(xr.tokenType() != QXmlStreamReader::Characters)
        {
          ELLE_DEBUG("no data");
          continue;
        }

        updater_info[name] = xr.text().toString();
        ELLE_DEBUG("added: [%s]:%s", name, updater_info[name]);
      }
    }
    else if (token == QXmlStreamReader::EndElement)
    {
      ELLE_DEBUG("end element: %s", xr.name().toString());
    }
    else if (token == QXmlStreamReader::Characters)
    {
      ELLE_DEBUG("Caracters....");
    }
    else
    {
      ELLE_DEBUG("unknown element: %s", token);
    }
  }

  if (xr.hasError())
  {
    emit update_error("unable to get update data"); return;
  }

  ELLE_TRACE("update data:")
    for (auto const& elem: updater_info.keys())
      ELLE_DEBUG("%s: %s", elem, updater_info[elem]);

  if (!updater_info.contains("version"))
  {
    emit update_error("unable to read the update file");
  }
  else if (updater_info["version"].toStdString() == INFINIT_VERSION)
  {
    emit no_update_available();
    ELLE_LOG("no update available");
    return;
  }

  if (updater_info.contains("url"))
  {
    this->_updater_url.setUrl(updater_info["url"]);
  }
  else
  {
    emit update_error("update file unavailable"); return;
  }

  ELLE_TRACE("an updater is available at %s", this->_updater_url);

#ifdef INFINIT_WINDOWS
  auto mandatory = true;

  if (updater_info.contains("rich_description"))
    this->_loading_dialog->body(updater_info["rich_description"]);
  else
    this->_loading_dialog->body(updater_info["description"]);

  if (mandatory)
    this->_loading_dialog->text("A mandatory update is available!");
  else
    this->_loading_dialog->text("An update is available!");

  emit update_available(mandatory);
#else
  ELLE_WARN("%s: linux update is not avalaible yet", *this);
  emit no_update_available();
#endif

}

void
Updater::_update(QNetworkReply* reply)
{
#ifdef INFINIT_WINDOWS
  ELLE_TRACE("Downloading new installer");

  if (!this->_installer->open(QIODevice::WriteOnly))
  {
    ELLE_ERR("unable to open location: %s", this->_installer->fileName());
    return;
  }

  this->_installer->write(this->_reply->readAll());
  this->_installer->close();

  ELLE_TRACE("running installer");

  {
    wchar_t psz_wdestfile[MAX_PATH];
    MultiByteToWideChar(
      CP_UTF8,
      0,
      QDir::toNativeSeparators(this->_installer->fileName())
        .toStdString().c_str(),
      -1,
      psz_wdestfile,
      MAX_PATH);

    HINSTANCE answer = ShellExecuteW(NULL, L"open", psz_wdestfile, NULL, NULL, SW_SHOW);
    if(answer > (HINSTANCE) 32)
    {
      this->_close_dialog();
      emit quit_request();
    }
    else
    {
      emit update_error(
        QString::fromStdString(
          elle::sprintf(
            "unable to run installer.\nYou can run it manually at %s",
            QDir::toNativeSeparators(this->_installer->fileName()))));
      return;
    }
  }
#else
  ELLE_WARN("%s: linux update is not avalaible yet", *this);
  emit no_update_available();
#endif
}

void
Updater::_on_error(QString const& error)
{
  ELLE_ERR("%s: an error occured: %s", *this, error);
  this->_close_dialog();
}

void
Updater::_close_dialog()
{
  ELLE_TRACE_SCOPE("%s: close dialog", *this);
  this->_loading_dialog->hide();
}

void
Updater::print(std::ostream& stream) const
{
  stream << "Updater";
}