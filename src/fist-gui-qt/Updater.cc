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
#include <QRegExp>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include <elle/AtomicFile.hh>
#include <elle/Version.hh>
#include <elle/system/system.hh>
#include <elle/assert.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/os/file.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>

#include <cryptography/oneway.hh>

#include <common/common.hh>

#include <version.hh>

#include <fist-gui-qt/Updater.hh>
#include <fist-gui-qt/utils.hh>

#ifdef INFINIT_WINDOWS
# include <stringapiset.h>
# include <shellapi.h>
#endif

ELLE_LOG_COMPONENT("infinit.FIST.Updater");

void
Updater::NetworkReplyLaterDeleter::operator () (QNetworkReply* reply) const
{
  reply->disconnect(SIGNAL(downloadProgress(qint64, qint64)));
  reply->deleteLater();
}

Updater::Updater(QUrl const& version_file_url,
                 QObject* parent)
  : QObject(parent)
  , _version_file_url(version_file_url)
  , _updater_url()
  , _installer_folder(
    QDir::toNativeSeparators(QString::fromStdString(common::infinit::home())))
  , _installer(
    new QFile(QDir::toNativeSeparators(
                this->_installer_folder.path() +
                QDir::separator() +
#ifdef INFINIT_WINDOWS
                "installer.exe"
#else
                "installer"
#endif
                )))
  , _loading_dialog(new LoadingDialog(this))
  , _network_manager(new QNetworkAccessManager(this))
  , _check_for_update_timer(nullptr)
  , _installer_downloaded(false)
{
  connect(this, SIGNAL(no_update_available()),
          this, SLOT(_remove_old_installer()));
  connect(this, SIGNAL(update_error(QString const&, QString const&)),
          this, SLOT(_on_error(QString const&, QString const&)));
  connect(this->_network_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(_handle_reply(QNetworkReply*)));
  connect(this->_loading_dialog, SIGNAL(quit_request()),
          this, SIGNAL(quit_request()));
  connect(this, SIGNAL(installer_ready()), SLOT(_set_installer_downloaded()));
}

void
Updater::download_update()
{
  ELLE_TRACE_SCOPE("%s: download updater application (%s)",
                   *this, this->_updater_url);
  emit update_available(false, this->_changelog);
  QNetworkRequest request;
  request.setUrl(this->_updater_url);
  request.setRawHeader(
    "User-Agent",
    QString::fromStdString(
      elle::sprintf("Windows %s", INFINIT_VERSION)).toUtf8());
  this->_reply.reset(this->_network_manager->get(request));
  connect(this->_reply.get(), SIGNAL(downloadProgress(qint64, qint64)),
          this, SIGNAL(download_progress(qint64, qint64)));
}

void
Updater::_remove_old_installer()
{
  // Delete the old installer file.
  if (this->_installer->exists())
  {
    ELLE_TRACE("%s: an old installer was present at %s: deleting it.",
               *this, this->_installer->fileName());
    this->_installer->remove();
  }
}

void
Updater::check_for_updates()
{
  ELLE_TRACE_SCOPE("%s: check for updates at %s", *this, this->_version_file_url);
  QNetworkRequest request;
  request.setUrl(this->_version_file_url);
  request.setRawHeader(
    "User-Agent",
    QString::fromStdString(elle::sprintf("Windows %s", INFINIT_VERSION)).toUtf8());
  this->_reply.reset(this->_network_manager->get(request));
}


void
Updater::_handle_reply(QNetworkReply* reply)
{
  ELLE_ASSERT(reply != nullptr);
  ELLE_TRACE_SCOPE("%s: reply from %s", *this, *reply);
  if (this->_reply->error() != QNetworkReply::NoError)
  {
    ELLE_WARN("something went wrong: %s", this->_reply->error());
    emit update_error("check your internet connection", reply->errorString());
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
    ELLE_DEBUG("finished response from %s", this->_updater_url);
    if (this->_reply->url() == this->_updater_url)
    {
      disconnect(this->_reply.get(), SIGNAL(downloadProgress(qint64, qint64)),
                 this, SIGNAL(download_progress(qint64, qint64)));
      this->_update(reply);
    }
    else if (this->_reply->url() == this->_version_file_url)
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
  QMap<QString, QString> updater_info;
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
    ELLE_DEBUG_SCOPE("read a new element");
    // Read the next element.
    QXmlStreamReader::TokenType token = xr.readNext();
    if(token == QXmlStreamReader::StartElement)
    {
      auto name = xr.name().toString();
      ELLE_DEBUG_SCOPE("%s:", name);
      xr.readNext();
      if (name == "rich_description")
      {
        xr.readNext();
        while (!(xr.tokenType() == QXmlStreamReader::EndElement && xr.name() == "rich_description"))
        {
          elle::SafeFinally next([&] { xr.readNext(); });
          if (xr.isStartElement())
            updater_info[name].append(QString::fromStdString(elle::sprintf("<%s>", xr.name().toString())));
          else if (xr.isEndElement())
            updater_info[name].append(QString::fromStdString(elle::sprintf("</%s>", xr.name().toString())));
          else if (xr.isCharacters())
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
    emit update_error("update to get update data", "unable to get update data");
    return;
  }
  ELLE_DEBUG("update data:")
    for (auto const& elem: updater_info.keys())
      ELLE_DEBUG("%s: %s", elem, updater_info[elem]);
  if (!updater_info.contains("version"))
  {
    emit update_error("unable to read the update file",
                      "unable to read the update file");
  }
  else if (!updater_info["version"].isEmpty())
  {
    static elle::Version current_version(
      INFINIT_VERSION_MAJOR, INFINIT_VERSION_MINOR, INFINIT_VERSION_SUBMINOR);
    ELLE_TRACE("current version: %s", current_version);
    QRegExp version("(\\d+)\\.(\\d+)\\.(\\d+)");
    version.indexIn(updater_info["version"]);
    elle::Version update_version(
      version.cap(1).toInt(), version.cap(2).toInt(), version.cap(3).toInt());
    ELLE_TRACE("update version: %s", update_version);
    if (update_version <= current_version)
    {
      emit no_update_available();
      ELLE_LOG("no update available");
      if (this->_check_for_update_timer == nullptr)
      {
        auto interval = 1000 * 60 * 60; // ms.
        ELLE_TRACE_SCOPE("launch update checker (check every %sms", interval);
        this->_check_for_update_timer = new QTimer(this);
        this->_check_for_update_timer->setInterval(interval);
        connect(this->_check_for_update_timer, SIGNAL(timeout()),
                this, SLOT(check_for_updates()));
        this->_check_for_update_timer->start();
        return;
      }
    }
    else
    {
      ELLE_LOG("current version is older than remote");
    }
  }
  if (updater_info.contains("url"))
  {
    this->_updater_url.setUrl(updater_info["url"]);
  }
  else
  {
    emit update_error("update file unavailable",
                      "update file unavailable"); return;
  }
  ELLE_TRACE("an update is available at %s", this->_updater_url);
#ifdef INFINIT_WINDOWS
  if (updater_info.contains("rich_description"))
    this->_changelog = updater_info["rich_description"];
  else
    this->_changelog = updater_info["description"];
  if (this->_installer->exists() && updater_info.contains("hash"))
  {
    auto check_local_installer = [&] () -> bool
    {
      auto const& file =
        QDir::toNativeSeparators(this->_installer->fileName()).toStdString();
      ELLE_TRACE_SCOPE("check local version of the installer %s", file);
      auto size = elle::os::file::size(file);
      auto const& str = elle::system::read_file_chunk(file, 0, size).string();
      infinit::cryptography::Plain installer_plain(str);
      auto hash = infinit::cryptography::oneway::hash(
        installer_plain, infinit::cryptography::oneway::Algorithm::sha1);
      std::string updater_hash(elle::format::hexadecimal::encode(hash.buffer()));
      ELLE_DEBUG("updater(%s) vs local(%s)", updater_info["hash"].toStdString(), updater_hash);
      return updater_info["hash"].toStdString() == updater_hash;
    };
    if (check_local_installer())
    {
      ELLE_TRACE("installer already downloaded");
      emit installer_ready();
    }
    else
    {
      ELLE_TRACE("old installer found in local");
      this->_remove_old_installer();
      this->download_update();
    }
  }
  else
  {
    ELLE_TRACE("no updater already present");
    this->download_update();
  }
#else
  ELLE_WARN("%s: linux update is not avalaible yet", *this);
  emit no_update_available();
#endif
}

void
Updater::_update(QNetworkReply* reply)
{
#ifdef INFINIT_WINDOWS
  ELLE_TRACE_SCOPE("installer downloaded");
  if (!this->_installer_folder.exists())
    if (!this->_installer_folder.mkpath(this->_installer_folder.path()))
      emit update_error(
        "unable to download installer",
        QString::fromStdString(
          elle::sprintf(
            "destination folder %s is unreachable.",
            QDir::toNativeSeparators(this->_installer_folder.path()))));
  if (!this->_installer->open(QIODevice::WriteOnly))
  {
    ELLE_ERR("unable to open location: %s", this->_installer->fileName());
    emit update_error(
        "unable to download installer",
        QString::fromStdString(
          elle::sprintf(
            "destination folder %s may be write protected.",
            QDir::toNativeSeparators(this->_installer->fileName()))));

    return;
  }
  {
    elle::SafeFinally close_stream([this] { this->_installer->close(); });
    this->_installer->write(this->_reply->readAll());
  }
  emit installer_ready();
#else
  ELLE_WARN("%s: linux update is not avalaible yet", *this);
  emit no_update_available();
#endif
}

void
Updater::_set_installer_downloaded()
{
  this->_installer_downloaded = true;
}

void
Updater::run_installer()
{
  ELLE_ASSERT(this->_installer != nullptr);
  ELLE_TRACE_SCOPE("running installer");
#ifdef INFINIT_WINDOWS
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

    wchar_t psz_parameters[256];
    MultiByteToWideChar(
      CP_UTF8,
      0,
      "/autoupdate",
      -1,
      psz_parameters,
      256);

    HINSTANCE answer = ShellExecuteW(
      NULL, L"open", psz_wdestfile, psz_parameters, NULL, SW_SHOW);
    if(answer > (HINSTANCE) 32)
    {
      this->_close_dialog();
      emit quit_request();
    }
    else
    {
      emit update_error(
        "unable to run installer",
        QString::fromStdString(
          elle::sprintf(
            "You can run it manually at %s",
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
Updater::_on_error(QString const& error, QString const& details)
{
  ELLE_ERR("%s: an error occured: %s (%s)", *this, error, details);
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
