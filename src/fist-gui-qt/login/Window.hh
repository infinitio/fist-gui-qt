#ifndef FIST_GUI_QT_LOGIN_WINDOW_HH
# define FIST_GUI_QT_LOGIN_WINDOW_HH

# include <memory>

# include <QFuture>
# include <QFutureWatcher>
# include <QKeyEvent>
# include <QLabel>
# include <QLineEdit>
# include <QMainWindow>
# include <QMovie>
# include <QPushButton>
# include <QVBoxLayout>

# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

# include <fist-gui-qt/InfinitDock.hh>
# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/SmoothLayout.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/login/fwd.hh>

namespace fist
{
  namespace login
  {
    class Window:
      public RoundShadowWidget
    {
      typedef RoundShadowWidget Super;

    private:
      ELLE_ATTRIBUTE(fist::State&, state);
    public:
      Window(fist::State& state,
             fist::gui::systray::Icon& systray,
             bool fill_email_and_password_fields = true,
             bool previous_session_crashed = false);
      ~Window();

      // Perform auto login if possible.
      // If not, show the window and focus the right field.
      void
      init();

      void keyPressEvent(QKeyEvent* event);


    private:
      // Check if the fields are filled in a proper way.
      bool
      _test_fields(bool test_fullname = false);

      bool
      _test_email_field(bool register_ = false);

    private slots:
      void
      _reduce();

      void
      _login();

      void
      _register();

      void
      _perform_login_or_register();

      // Login register and login attempt are pretty similar in term of
      // implementation, but I rather split both behavior, so each one use it's
      // QFuture and QFuture watcher, signals, and slots.
      void
      _login_attempt(gap_Status);

      void
      _register_attempt(gap_Status);

      void
      _internet_issue(QString const&);

    public slots:
      void
      try_auto_login();

      void
      clear_credentials(bool clear_cookies = true);

    Q_SIGNALS:
      void
      logged_in();

      void
      registered();

      void
      quit_request();

      void
      version_rejected();

      void
      login_attempt();

      void
      login_failed();

      void
      register_attempt();

      void
      register_failed();

    public slots:
      void
      update_available(bool mandatory,
                       QString const& changelog);

      void
      download_progress(qint64 downloaded,
                        qint64 total_size);

      void
      download_ready();

    Q_SIGNALS:
      void
      update_application();

    private:
      void
      focusInEvent(QFocusEvent* event) override;

      void
      paintEvent(QPaintEvent*) override;

      void
      mode(Mode mode,
           bool clear_message = true);

    private slots:
      void
      _systray_activated(QSystemTrayIcon::ActivationReason reason);

    private:
      ELLE_ATTRIBUTE(bool, previous_session_crashed);
      ELLE_ATTRIBUTE(fist::gui::systray::Icon&, systray);
      ELLE_ATTRIBUTE_R(Mode, mode);
      ELLE_ATTRIBUTE(QLabel*, signup_tabber);
      ELLE_ATTRIBUTE(QLabel*, login_tabber);
      // Can be forgot password or help depending on the view.
      QWidget const*
      active() const;
      ELLE_ATTRIBUTE(QLabel*, separator);
      ELLE_ATTRIBUTE(QMovie*, loading_icon);
      ELLE_ATTRIBUTE(QLabel*, loading);
      ELLE_ATTRIBUTE(QWidget*, facebook_email_info);
      ELLE_ATTRIBUTE(QLineEdit*, fullname_field);
      ELLE_ATTRIBUTE(QLineEdit*, email_field);
      ELLE_ATTRIBUTE(QLineEdit*, password_field);
      ELLE_ATTRIBUTE(QLabel*, message_field);

      ELLE_ATTRIBUTE(QLabel*, help_link);
      ELLE_ATTRIBUTE(QLabel*, forgot_password_link);
      ELLE_ATTRIBUTE(QLabel*, version_field);
      ELLE_ATTRIBUTE(QPushButton*, login_button);
      ELLE_ATTRIBUTE(QPushButton*, facebook_button);

      ELLE_ATTRIBUTE(std::unique_ptr<facebook::ConnectWindow>, facebook_window);
      ELLE_ATTRIBUTE(bool, facebook_connect_attempt);
    public:
      void
      closeEvent(QCloseEvent* event) override;

      bool
      eventFilter(QObject *obj, QEvent *event) override;

    public Q_SLOTS:
      void
      set_message(QString const& message,
                  QString const& tooltip = "",
                  bool critical = true);

      void
      set_version();

      void
      _enable();

      void
      _disable();

      void
      _set_enabled(bool val);

    private:
      // Save the password into user settings.
      void
      _save_password(QString const& email,
                     QString const& password);

      // Get the password from settings.
      QString
      _saved_password(QString const& email) const;

    private:
      void
      _update_geometry();
  /*-----------.
  | Visibility |
  `-----------*/
      void
      showEvent(QShowEvent*) override;

      void
      hideEvent(QHideEvent*) override;

  /*----------.
  | Printable |
  `----------*/
    private:
      void
      print(std::ostream& stream) const override;

  /*---------.
  | Facebook |
  `---------*/
    private slots:
      void
      fb(QString const& code);

      void
      _ask_for_facebook_email();

      void
      facebook_connect_failed(QString const& code);

      void
      launch_facebook_connect();

  /*------.
  | Proxy |
  `------*/
    private:
      ELLE_ATTRIBUTE(QLabel*, proxy);
      ELLE_ATTRIBUTE(std::unique_ptr<QDialog>, proxy_dialog);
    private slots:
      void
      _apply_proxy_configuration();

    private:
      Q_OBJECT
    };
  }
}

#endif // !LOGINWINDOW_HH
