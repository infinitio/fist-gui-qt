#ifndef FIST_GUI_QT_LOGIN_WINDOW_HH
# define FIST_GUI_QT_LOGIN_WINDOW_HH

# include <memory>

# include <QLabel>
# include <QKeyEvent>
# include <QPushButton>
# include <QVBoxLayout>
# include <QLineEdit>
# include <QMainWindow>
# include <QMovie>
# include <QFuture>
# include <QFutureWatcher>

# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/login/fwd.hh>
# include <fist-gui-qt/InfinitDock.hh>
# include <fist-gui-qt/RoundShadowWidget.hh>
# include <fist-gui-qt/SmoothLayout.hh>

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

      void keyPressEvent(QKeyEvent* event);

    private:
      // Check if the fields are filled in a proper way.
      bool
      _test_fields(bool test_fullname = false);

    private slots:
      void
      _reduce();

      void
      _login();

      void
      _register();

      // Login register and login attempt are pretty similar in term of
      // implementation, but I rather split both behavior, so each one use it's
      // QFuture and QFuture watcher, signals, and slots.
      void
      _login_attempt();

      void
      _register_attempt();
    public slots:
      void
      try_auto_login();

    Q_SIGNALS:
      void
      logged_in();

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

    private:
      ELLE_ATTRIBUTE(bool, previous_session_crashed);
      ELLE_ATTRIBUTE(fist::gui::systray::Icon&, systray);
      ELLE_ATTRIBUTE_Rw(Mode, mode);

      ELLE_ATTRIBUTE(SmoothLayout*, body);
      ELLE_ATTRIBUTE(QLabel*, info);
      ELLE_ATTRIBUTE(QLineEdit*, fullname_field);
      ELLE_ATTRIBUTE(QLineEdit*, email_field);
      ELLE_ATTRIBUTE(QLineEdit*, password_field);
      ELLE_ATTRIBUTE(QLabel*, message_field);

      ELLE_ATTRIBUTE(IconButton*, quit_button);
      // Can be forgot password or help depending on the view.
      ELLE_ATTRIBUTE(QLabel*, help_link);
      // Can be need or already have an account, depending on the view.
      ELLE_ATTRIBUTE(QLabel*, switch_mode);

      ELLE_ATTRIBUTE(QLabel*, version_field);
      ELLE_ATTRIBUTE(Footer*, footer);

      ELLE_ATTRIBUTE(QFuture<gap_Status>, login_future);
      ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, login_watcher);
      ELLE_ATTRIBUTE(QFuture<gap_Status>, register_future);
      ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, register_watcher);
    public:
      void
      closeEvent(QCloseEvent* event) override;

      bool
      eventFilter(QObject *obj, QEvent *event) override;

    public Q_SLOTS:
      void
      set_message(QString const& message,
                  QString const& tooltip,
                  bool critical = true);

      void
      set_version();

      void
      _enable();

      void
      _disable(bool disable_fullname = false);

    private:
      // Save the password into user settings.
      void
      _save_password(QString const& email,
                     QString const& password);

      // Get the password from settings.
      QString
      _saved_password(QString const& email) const;

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

    private:
      Q_OBJECT
    };
  }
}

#endif // !LOGINWINDOW_HH
