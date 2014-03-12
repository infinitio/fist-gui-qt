#ifndef UPDATER_HH
# define UPDATER_HH

# include <memory>

# include <QDialog>
# include <QDir>
# include <QFile>
# include <QNetworkAccessManager>
# include <QNetworkReply>
# include <QObject>
# include <QUrl>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <fist-gui-qt/LoadingDialog.hh>

class Updater:
  public QObject,
  public elle::Printable
{
public:
  Updater(QUrl const& version_url =
            QString("http://www.infinit.io/files/windows/update.json"),
          QObject* parent = nullptr);

public Q_SLOTS:
  // Run the update.
  // Will close the application.
  void
  update();

private:
  void
  _update(QNetworkReply* reply);

public Q_SLOTS:
  // Check if an update is available.
  void
  check_for_updates();

private:
  void
  _check_if_up_to_date(QNetworkReply* reply);

Q_SIGNALS:
  // Signal that an update is available.
  void
  update_available(bool mandatory);

  // Signal that no update is available.
  void
  no_update_available();

  // Signal that the update starts.
  void
  updating();

  // Signal that an error has occured.
  void
  update_error(QString const&);

private Q_SLOTS:
  void
  _handle_reply(QNetworkReply*);

  void
  _on_error(QString const&);

  void
  _close_dialog();

Q_SIGNALS:
  void
  quit_request();


private:
  ELLE_ATTRIBUTE(QUrl, version_url);
  ELLE_ATTRIBUTE(QUrl, updater_url);

  ELLE_ATTRIBUTE(QDir, installer_folder);
  ELLE_ATTRIBUTE(std::unique_ptr<QFile>, installer);
  ELLE_ATTRIBUTE_X(LoadingDialog*, loading_dialog);
  ELLE_ATTRIBUTE(QNetworkAccessManager*, network_manager);
  ELLE_ATTRIBUTE(std::unique_ptr<QNetworkReply>, reply);
/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

#endif
