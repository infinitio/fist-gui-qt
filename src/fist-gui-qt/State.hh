#ifndef FIST_GUI_QT_STATE_HH
# define FIST_GUI_QT_STATE_HH

# include <functional>
# include <vector>

# include <QDateTime>
# include <QFuture>
# include <QtConcurrentRun>
# include <QFutureWatcher>
# include <QRegExp>
# include <QString>
# include <QVector>

# include <boost/multi_index_container.hpp>
# include <boost/multi_index/sequenced_index.hpp>
# include <boost/multi_index/ordered_index.hpp>
# include <boost/multi_index/mem_fun.hpp>

# include <elle/attribute.hh>

# include <fist-gui-qt/model/Link.hh>
# include <fist-gui-qt/model/User.hh>
# include <fist-gui-qt/model/Transaction.hh>

# include <surface/gap/gap.hh>

namespace fist
{
  // This class provide a wrapper for the gap_State.
  // Until we stop using the c api, we need a duplicated model.
  // So that class mainly wrap the Users, the Transactions and the Links.
  class State:
    public QObject
  {
  public:
    State();
    virtual
    ~State();
    // Handle gap destruction automatically.
    struct GapDeleter
    {
      void
      operator () (gap_State* state) const;
    };
    typedef std::unique_ptr<gap_State, GapDeleter> StatePtr;
    ELLE_ATTRIBUTE(StatePtr, state);

  public:
    gap_State*
    state() const
    {
      return this->_state.get();
    }


    ELLE_ATTRIBUTE(QFuture<gap_Status>, login_future);
    ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, login_watcher);
    ELLE_ATTRIBUTE(QFuture<gap_Status>, register_future);
    ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, register_watcher);

  public:
    void
    login(std::string const& email,
          std::string const& password);

    void
    facebook_connect(std::string const& token,
                     std::string const& email = "");

  private slots:
    void
    _on_login_result_ready();
  signals:
    void
    login_result(gap_Status);
  public:
    void
    register_(std::string const& fullname,
              std::string const& email,
              std::string const& password);
  private slots:
    void
    _on_register_result_ready();
  signals:
    void
    register_result(gap_Status);

  public slots:
    void
    on_logged_in();
    void
    send_metric(UIMetricsType metric,
                std::unordered_map<std::string, std::string> const& additional =
                  std::unordered_map<std::string, std::string>());

  private slots:
    void
    _poll();

  public:
    void
    on_connection_changed(
      bool status, bool still_retrying, std::string last_error);

  signals:
    void
    connection_enable();
    void
    internet_issue(QString const&);
    void
    kicked_out(QString const&);
    void
    new_download_folder_needed();

  public:
    /*-----------.
    | Ghost code |
    `-----------*/
    void
    use_ghost_code(std::string const& fullname);

    void
    cancel_ghost_code();
  private slots:
    void
    _on_ghost_code_result_ready();
  signals:
    void
    ghost_code_result(gap_Status);

  private:
    ELLE_ATTRIBUTE(QFuture<gap_Status>, ghost_code_future);
    ELLE_ATTRIBUTE(QFutureWatcher<gap_Status>, ghost_code_watcher);

  public:
    /*------.
    | Users |
    `------*/
    // Me.
    ELLE_ATTRIBUTE_P(uint32_t, my_id, mutable);
  public:
    uint32_t
    my_id() const;
    ELLE_ATTRIBUTE_P(std::string, device, mutable);
  public:
    std::string
    device() const;
  public:
    model::User const&
    me();

  public:
    std::vector<model::Device>
    devices() const;

  public:
    // typedef std::vector<model::User const&> Users;
    typedef std::vector<uint32_t> Users;

    // Return every swaggers.
    Users
    swaggers(
      std::function<bool (model::User const&)> filter = [] (model::User const&) { return true; });

    // Return a subset of the swaggers.
    Users
    swaggers(QString const& filter);

    // Launch a search (long operation).
    // Directly returns the swaggers matching the search.
    // When the operation is finished, the watcher will signal it (and is
    // accessible in the State api).
    Users
    search(QString const& filter);

    // Cancel the search operation.
    void
    cancel_search();

    Users
    results();

    model::User const&
    user(uint32_t user_id);

    uint32_t
    user_id(std::string const& email);

    typedef std::unordered_map<uint32_t, std::unique_ptr<model::User>> UserModels;
    ELLE_ATTRIBUTE_R(UserModels, users);
    typedef std::vector<uint32_t> Results;
    typedef QFuture<Results> FutureSearchResult;
    typedef QFutureWatcher<Results> SearchResultWatcher;
    ELLE_ATTRIBUTE(FutureSearchResult, search_future);
    ELLE_ATTRIBUTE_X(SearchResultWatcher, search_watcher);
    ELLE_ATTRIBUTE(Results, last_results);

    void
    on_avatar_available(uint32_t);
    void
    on_user_status_changed(uint32_t, bool status);
    void
    on_user_updated(surface::gap::User const&);
    void
    on_swagger_deleted(uint32_t id);

  signals:
    void
    search_results_ready();

  private slots:
    void
    _on_search_results_ready();

  public:
    QString
    download_folder() const;

    void
    download_folder(QString const&);

    /*-------------.
    | Transactions |
    `-------------*/
    typedef boost::multi_index::multi_index_container<
      model::Transaction,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
          boost::multi_index::const_mem_fun<model::Model, uint32_t, &model::Model::id>
          >,
        boost::multi_index::ordered_non_unique<
          boost::multi_index::const_mem_fun<model::Transaction, QDateTime const&, &model::Transaction::mtime>
          >
        >
      > Transactions;
    ELLE_ATTRIBUTE_R(Transactions, transactions);
    ELLE_ATTRIBUTE_Rw(size_t, acceptable_transactions);
    ELLE_ATTRIBUTE_Rw(size_t, running_transactions);
    // Some method are static in order to provide a prototype matching  callback
    // the the C api.
    // A global instance of state is accessible in order to allow the bouncing
    // static function to operate on the instance of State.
  public:
    void
    on_peer_transaction_updated(surface::gap::PeerTransaction const& tr);
    void
    on_link_updated(surface::gap::LinkTransaction const& tr);

    model::Transaction const&
    transaction(uint32_t id);

    void
    _compute_active_transactions();

  private:
    surface::gap::PeerTransaction
    _force_transaction_status(uint32_t id, gap_TransactionStatus status) const;

    surface::gap::LinkTransaction
    _force_link_status(uint32_t id, gap_TransactionStatus status) const;

  public slots:
    void
    on_transaction_accepted(uint32_t id);
    void
    on_transaction_rejected(uint32_t id);
    void
    on_transaction_canceled(uint32_t id);
    void
    on_transaction_deleted(uint32_t id);
    void
    open_file(uint32_t id);
  signals:
    void
    new_transaction(uint32_t id);
    void
    transaction_updated(uint32_t id);
    void
    acceptable_transactions_changed(size_t);
    void
    running_transactions_changed(size_t);

  private:
    /*------.
    | Links |
    `------*/
    typedef boost::multi_index::multi_index_container<
      model::Link,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
          boost::multi_index::const_mem_fun<model::Model, uint32_t, &model::Model::id>
          >,
        boost::multi_index::ordered_non_unique<
          boost::multi_index::const_mem_fun<model::Link, QDateTime const&, &model::Link::mtime>
          >
        >
      > Links;
    ELLE_ATTRIBUTE_R(Links, links);
    ELLE_ATTRIBUTE_Rw(size_t, active_links);
    ELLE_ATTRIBUTE(std::unique_ptr<QTimer>, poll_timer);
  public:
    model::Link const&
    link(uint32_t id);

  signals:
    void
    new_link(uint32_t id);
    void
    link_updated(uint32_t id);
    void
    active_links_changed(size_t);
  private:
    void
    _compute_active_links();
  public:
    QString
    facebook_app_id() const;

  private:
    Q_OBJECT;
  };
}

#endif
