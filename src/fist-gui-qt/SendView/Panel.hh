#ifndef FIST_GUI_QT_SEND_VIEW_PANEL_HH
# define FIST_GUI_QT_SEND_VIEW_PANEL_HH

# include <QUrl>
# include <QWidget>
# include <QFuture>
# include <QFutureWatcher>

# include <unordered_map>
# include <vector>
# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <surface/gap/fwd.hh>

# include <fist-gui-qt/SendView/Files.hh>
# include <fist-gui-qt/SendView/Footer.hh>
# include <fist-gui-qt/SendView/Message.hh>
# include <fist-gui-qt/SendView/Users.hh>
# include <fist-gui-qt/Panel.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/fwd.hh>
# include <fist-gui-qt/gui/TabWidget.hh>
# include <fist-gui-qt/ModeSelector.hh>

namespace fist
{
  namespace sendview
  {
    class Panel
      : public ::Panel
      , public ModeSelector
    {

    /*------.
    | Types |
    `------*/
    public:
      typedef Panel Self;
      typedef ::Panel Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Panel(fist::State& state);

    /*------.
    | Files |
    `------*/
    signals:
      void
      drag_entered();

      void
      drag_left();

      void
      shown();

      void
      peer_found();

      void
      file_added();

    private slots:

      bool
      _check_files();

      void
      _send(bool screenshot = false);

      void
      _pick_user();

      void
      _canceled();

    public slots:
      void
      avatar_available(uint32_t uid);

    private slots:
      void
      _dropped();

    private:
      void
      _mode_implementation() override;

    private slots:
      void
      p2p_mode();

      void
      link_mode();

      void
      clear();

    public slots:
      void
      send(bool screenshot = false);

    Q_SIGNALS:
      void
      switch_signal();

      void
      choose_files();

      void
      sent();

      void
      canceled();

    /*--------------.
    | Drag and drop |
    `--------------*/
    public:
      void
      dragEnterEvent(QDragEnterEvent *event) override;

      void
      dragLeaveEvent(QDragLeaveEvent *event) override;

      void
      dropEvent(QDropEvent *event) override;

    private:
      ELLE_ATTRIBUTE(fist::State&, state);
      ELLE_ATTRIBUTE_R(gui::TabWidget*, tabs);
      ELLE_ATTRIBUTE_R(Users*, users);
      ELLE_ATTRIBUTE_R(Message*, message);
      ELLE_ATTRIBUTE_R(Files*, file_adder);
      ELLE_ATTRIBUTE(gui::Tab*, transaction_tab);
      ELLE_ATTRIBUTE(gui::Tab*, link_tab);

    protected:
      void
      keyPressEvent(QKeyEvent* event) override;

    /*-------.
    | Layout |
    `-------*/
    private:
      std::unordered_map<uint32_t, std::unique_ptr<model::User>> _user_models;


    private:
      void
      showEvent(QShowEvent*) override;

      void
      hideEvent(QHideEvent*) override;

    private:
      void
      focusInEvent(QFocusEvent* event) override;

    /*-------.
    | Footer |
    `-------*/
    public:
      Footer*
      footer() override;

    private:
      Q_OBJECT;

    public:
        /*----------.
        | Printable |
        `----------*/
      void
      print(std::ostream& stream) const override;
    };
  }
}

#endif
