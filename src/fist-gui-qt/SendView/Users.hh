#ifndef FIST_GUI_QT_SEND_VIEW_USERS_HH
# define FIST_GUI_QT_SEND_VIEW_USERS_HH

# include <unordered_set>
# include <memory>

# include <QLineEdit>
# include <QKeyEvent>
# include <QLabel>
# include <QTimer>
# include <QMovie>

# include <fist-gui-qt/utils.hh>
# include <fist-gui-qt/ListWidget.hh>
# include <fist-gui-qt/HorizontalSeparator.hh>
# include <fist-gui-qt/State.hh>

namespace fist
{
  namespace sendview
  {
    class SearchField:
      public QLineEdit
    {
      typedef QLineEdit Super;
    public:
      SearchField(QWidget* parent = nullptr);
      SearchField(QString const& text, QWidget* parent = nullptr);

    public:
      void
      keyPressEvent(QKeyEvent * event) override;
      void
      focusInEvent(QFocusEvent* event) override;

    signals:
      void
      up_pressed();
      void
      down_pressed();
    private:
      Q_OBJECT;
    };

    class Recipient
      : public elle::Printable
    {
      ELLE_ATTRIBUTE_R(uint32_t, id);
      ELLE_ATTRIBUTE_R(boost::optional<model::Device>, device);
      ELLE_ATTRIBUTE_R(boost::optional<QString>, email);

    public:
      Recipient(uint32_t id,
                boost::optional<model::Device> device = boost::none,
                QString const& email = "")
        : _id(id)
        , _device(device)
        , _email(email.isEmpty()
                 ? boost::optional<QString>{}
                 : boost::optional<QString>{email})
      {}

      bool
      to_device() const
      {
        return static_cast<bool>(this->_device);
      }

      bool
      to_email() const
      {
        return static_cast<bool>(this->_email);
      }

      operator uint32_t() const
      {
        return this->id();
      }

      bool
      operator == (uint32_t id) const;

      bool
      operator == (QString const& value) const;

      bool
      operator == (Recipient const& rec) const;

      void
      print(std::ostream& out) const override
      {
        if (this->to_device())
          out << "Device(" << this->device().get() << ")";
        else if (this->to_email())
          out << "Email(" << this->_email.get() << ")";
        else
          out << "User(" << this->id() << ")";
      }
    };
  }
}

namespace std
{
  template<>
  struct hash<fist::sendview::Recipient>
  {
    size_t
    operator ()(fist::sendview::Recipient const& recipient) const;
  };
}

namespace fist
{
  namespace sendview
  {
    class Users:
      public QWidget
    {
      typedef fist::State::Users UserList;
      typedef QWidget Super;

  /*-------------.
  | Construction |
  `-------------*/
    public:
      Users(fist::State& state,
            QWidget* owner);

  /*--------.
  | Display |
  `--------*/
    public:
      ELLE_ATTRIBUTE(fist::State&, state);
      // Icon.
      ELLE_ATTRIBUTE(QPixmap, magnifier);
      ELLE_ATTRIBUTE(QLabel*, icon);
      // Search field.
      ELLE_ATTRIBUTE_X(SearchField*, search_field);
      // Resutls.
      typedef std::vector<Recipient> Results;
      ELLE_ATTRIBUTE(Results, results);
      typedef std::vector<Recipient> Recipients;
      ELLE_ATTRIBUTE_R(Recipients, recipients);
      ELLE_ATTRIBUTE(HorizontalSeparator*, separator);
      ELLE_ATTRIBUTE(ListWidget*, users);
      ELLE_ATTRIBUTE(int, max);

    private:
      void
      _compute_results(UserList const& users,
                       bool no_self);

      void
      _add_result(Recipient const& rec, bool foo);

    public:
      void
      set_icon(QPixmap const& pixmap);

      void
      set_text(QString const& text);

      QString
      text() const;

      void
      clear_search(bool final = false);

      void
      clear_results ();

      void
      clear(bool final = false);

      bool
      peer_valid() const;

    protected:
      void
      keyPressEvent(QKeyEvent* event) override;
      void
      showEvent(QShowEvent* event) override;
      void
      hideEvent(QHideEvent* event) override;

    private:
      ELLE_ATTRIBUTE(QMutex, user_mutex);
    Q_SIGNALS:
      void
      search_field_focused();

      void
      search_field_unfocused();

      void
      up_pressed();

      void
      down_pressed();

      void
      return_pressed();

      void
      search_ready(QString const& text);

      void
      search_changed(QString const& text);

      void
      peer_found();

      void
      send_metric(UIMetricsType,
                  std::unordered_map<std::string, std::string> const&);


    private:
      // std::shared_ptr<_SearchResultWidget>
      // _add_search_result(model::User const& model);

      // void
      // _add_device_search_result(model::Device const& device,
      //                           bool selected);

    private slots:
      void
      text_changed(QString const& text);

      void
      _select_first_user();

      void
      focusInEvent(QFocusEvent* event) override;

      void
      set_users(UserList const& users, bool local);

    private:
      bool
      _add_recipient(Recipient const&);

      bool
      _remove_recipient(Recipient const&);

    private slots:
      void
      _add_peer(uint32_t);

      void
      _remove_peer(uint32_t);

      void
      _add_device(uint32_t, QString const&);

      void
      _remove_device(uint32_t, QString const&);

      void
      _add_email(QString const&);

      void
      _remove_email(QString const&);

    private:
      bool
      eventFilter(QObject *obj, QEvent *event) override;

  /*-------.
  | Layout |
  `-------*/
    public:
      virtual
      QSize
      sizeHint() const override;

      virtual
      QSize
      minimumSizeHint() const override;

    private:
      Q_OBJECT;
    };
  }
}
#endif
