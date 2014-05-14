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

# include <fist-gui-qt/UserModel.hh>
# include <fist-gui-qt/TransactionModel.hh>
// # include <fist-gui-qt/LinkModel.hh>

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

    ELLE_ATTRIBUTE_R(gap_State*, state);

    /*------.
      | Users |
      `------*/
    typedef QVector<UserModel*> Users;

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

    typedef std::unordered_map<uint32_t, std::unique_ptr<UserModel>> UserModels;
    ELLE_ATTRIBUTE_R(UserModels, users);
    typedef QFuture<std::vector<uint32_t>> FutureSearchResult;
    typedef QFutureWatcher<std::vector<uint32_t>> SearchResultWatcher;
    ELLE_ATTRIBUTE(FutureSearchResult, search_future);
    ELLE_ATTRIBUTE_X(SearchResultWatcher, search_watcher);
    ELLE_ATTRIBUTE(std::vector<uint32_t>, last_results);

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
      TransactionModel,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
          boost::multi_index::const_mem_fun<TransactionModel, uint32_t, &TransactionModel::id>
          >,
        boost::multi_index::ordered_non_unique<
          boost::multi_index::const_mem_fun<TransactionModel, QDateTime const&, &TransactionModel::mtime>
          >
        >
      > TransactionModels;
    ELLE_ATTRIBUTE_R(TransactionModels, transactions);

  private:
    Q_OBJECT;
  };
}

#endif
