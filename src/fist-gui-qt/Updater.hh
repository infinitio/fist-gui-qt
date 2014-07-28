#ifndef FIST_GUI_QT_UPDATER_HH
# define FIST_GUI_QT_UPDATER_HH

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
            QString("http://www.download.infinit.io/windows/32/update.xml"),
          QObject* parent = nullptr);

  virtual
  ~Updater() = default;

public Q_SLOTS:
  // Start update download.
  void
  download_update();

  // Run the update.
  // Will close the application.
  void
  run_installer();

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

private slots:
  void
  _remove_old_installer();

Q_SIGNALS:
  // Signal that an update is available.
  void
  update_available(bool mandatory,
                   QString const& changelog);

  // Signal that no update is available.
  void
  no_update_available();

  // Signal that the update starts.
  void
  updating();

  // Signal that an error has occured.
  void
  update_error(QString const& message,
               QString const& detail);

  // Signal update download progression.
  void
  download_progress(qint64 progress,
                    qint64 total);

  // Signal when the updater is ready to be used.
  void
  installer_ready();

public slots:
  void
  version_rejected();

private slots:
  void
  _set_installer_downloaded();

private Q_SLOTS:
  void
  _handle_reply(QNetworkReply*);

  void
  _on_error(QString const& message, QString const&);

  void
  _close_dialog();

Q_SIGNALS:
  void
  quit_request();


private:
  ELLE_ATTRIBUTE(QUrl, version_file_url);
  ELLE_ATTRIBUTE(QUrl, updater_url);

  ELLE_ATTRIBUTE(QDir, installer_folder);
  ELLE_ATTRIBUTE(std::unique_ptr<QFile>, installer);
  ELLE_ATTRIBUTE_X(LoadingDialog*, loading_dialog);
  ELLE_ATTRIBUTE(QNetworkAccessManager*, network_manager);
  ELLE_ATTRIBUTE_R(QString, changelog);
  /*
    * From QNetworkAccessManager docs about request destruction *
    Note: After the request has finished, it is the responsibility of the user
    to delete the QNetworkReply object at an appropriate time. Do not directly
    delete it inside the slot connected to finished(). You can use
    the deleteLater() function.
  */
  struct NetworkReplyLaterDeleter
  {
    void
    operator () (QNetworkReply* reply) const;
  };
  typedef
    std::unique_ptr<QNetworkReply, NetworkReplyLaterDeleter> NetworkReplyPtr;
  ELLE_ATTRIBUTE(NetworkReplyPtr, reply);
  ELLE_ATTRIBUTE(QTimer*, check_for_update_timer);
  ELLE_ATTRIBUTE_R(bool, installer_downloaded);
/*----------.
| Printable |
`----------*/
  void
  print(std::ostream& stream) const override;

private:
  Q_OBJECT;
};

#endif
