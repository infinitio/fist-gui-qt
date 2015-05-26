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
    {
      ELLE_ATTRIBUTE_R(uint32_t, id);
      ELLE_ATTRIBUTE_R(boost::optional<std::string>, device_uuid);

    public:
      Recipient(uint32_t id,
                QString const& device_name = "")
        : _id(id)
        , _device_uuid(device_name.isEmpty()
                       ? boost::optional<std::string>{}
                       : boost::optional<std::string>{device_name.toStdString()})
      {}

      operator uint32_t() const
      {
        return this->id();
      }

      bool
      operator == (uint32_t id) const
      {
        return this->id() == id;
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
      ELLE_ATTRIBUTE(QMovie*, loading_icon);
      ELLE_ATTRIBUTE(QLabel*, icon);
      // Search field.
      ELLE_ATTRIBUTE_X(SearchField*, search_field);
      // Resutls.
      typedef std::unordered_map<uint32_t, std::shared_ptr<SearchResultWidget>> Results;
      ELLE_ATTRIBUTE(Results, results);
      typedef std::unordered_set<Recipient> Recipients;
      ELLE_ATTRIBUTE_R(Recipients, recipients);
      ELLE_ATTRIBUTE(HorizontalSeparator*, separator);
      ELLE_ATTRIBUTE(ListWidget*, users);

    public:
      void
      set_icon(QPixmap const& pixmap);

      void
      set_icon(QMovie& movie);

      void
      set_text(QString const& text);

      QString
      text() const;

      void
      clear_search();

      void
      clear_results ();

      void
      clear();

      bool
      peer_valid() const;

    protected:
      void
      keyPressEvent(QKeyEvent* event) override;
      void
      showEvent(QShowEvent* event) override;
      void
      hideEvent(QHideEvent* event) override;

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
      void
      _add_search_result(model::User const& model);

      void
      _add_device_search_result(model::User const& me);

    private slots:
      void
      delay_expired();

      void
      text_changed(QString const& text);

      void
      _select_first_user();

      void
      focusInEvent(QFocusEvent* event) override;

      void
      _set_users();

      void
      set_users(UserList const& users, bool local);

      void
      _add_peer(uint32_t);

      void
      _remove_peer(uint32_t);

      void
      _add_device(uint32_t, QString const&);

      void
      _remove_device(uint32_t, QString const&);

    private:
      QTimer _search_delay;

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
