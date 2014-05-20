#ifndef FIST_GUI_QT_STATE_HH
# define FIST_GUI_QT_STATE_HH

# include <QDateTime>
# include <QFuture>
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
    State(gap_State* state);
    virtual
    ~State() = default;
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

  private slots:
    void
    _poll();
  public slots:
    void
    on_logged_in();

  public:
    /*------.
    | Users |
    `------*/
    typedef QVector<model::User*> Users;

    // Return every swaggers.
    Users
    swaggers();

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

    typedef std::unordered_map<uint32_t, std::unique_ptr<model::User>> UserModels;
    ELLE_ATTRIBUTE_R(UserModels, users);
    typedef QFuture<std::vector<uint32_t>> FutureSearchResult;
    typedef QFutureWatcher<std::vector<uint32_t>> SearchResultWatcher;
    ELLE_ATTRIBUTE(FutureSearchResult, search_future);
    ELLE_ATTRIBUTE_X(SearchResultWatcher, search_watcher);
    ELLE_ATTRIBUTE(std::vector<uint32_t>, last_results);

    static
    void
    avatar_available_callback(uint32_t id);
    void
    on_avatar_available_callback(uint32_t);

    static
    void
    user_status_callback(uint32_t id, gap_UserStatus status);
    void
    on_user_status_callback(uint32_t, gap_UserStatus status);

  signals:
    void
    results_ready();

  private slots:
    void
    _on_results_ready();

  public:
    // static
    // void
    // user_status_cb(uint32_t id,
    //                gap_UserStatus const status);

    // static
    // void
    // avatar_available_cb(uint32_t id);

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
    ELLE_ATTRIBUTE_Rw(unsigned int, active_transactions);

    // Some method are static in order to provide a prototype matching  callback
    // the the C api.
    // A global instance of state is accessible in order to allow the bouncing
    // static function to operate on the instance of State.
  public:
    static
    void
    transaction_callback(uint32_t id,
                         gap_TransactionStatus status);
    void
    on_transaction_callback(uint32_t,
                            gap_TransactionStatus);

    model::Transaction const&
    transaction(uint32_t id);

    void
    _compute_active_transactions();

  public slots:
    void
    on_transaction_accepted(uint32_t id);
    void
    on_transaction_rejected(uint32_t id);
    void
    on_transaction_canceled(uint32_t id);
    void
    open_file(uint32_t id);
  signals:
    void
    new_transaction(uint32_t id);
    void
    transaction_updated(uint32_t id);
    void
    active_transactions_changed(unsigned int);

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

  private:
    Q_OBJECT;
  };
}

#endif
