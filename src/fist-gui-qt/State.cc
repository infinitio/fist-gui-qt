# include <QtConcurrentRun>
# include <QVector>

# include <elle/log.hh>

# include <fist-gui-qt/UserModel.hh>
# include <fist-gui-qt/TransactionModel.hh>
# include <fist-gui-qt/State.hh>
# include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.State");

namespace fist
{

  namespace
  {
    static const QRegExp email_checker(regexp::email,
                                       Qt::CaseInsensitive);
  }

  State::State(gap_State* state):
    _state(state)
  {
    ELLE_TRACE("load swaggers")
    {
      uint32_t* swaggers = gap_swaggers(this->_state);
      for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
        this->_users[swaggers[i]].reset(new UserModel(this->_state, swaggers[i]));
      gap_swaggers_free(swaggers);
    }

    ELLE_TRACE("load transactions")
    {
      uint32_t* trs = gap_transactions(this->_state);
      for (uint32_t i = 0; trs[i] != gap_null(); ++i)
        this->_transactions.emplace(this->_state, trs[i]);
      gap_transactions_free(trs);
    }

    connect(&this->_search_watcher, SIGNAL(finished()),
            this, SLOT(_on_results_ready()));
  }

  State::Users
  State::swaggers()
  {
    uint32_t* swaggers = gap_swaggers(this->_state);

    State::Users res;
    for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
    {
      ELLE_ASSERT_CONTAINS(this->_users, swaggers[i]);
      auto const& user = this->_users.at(swaggers[i]);
      res.append(user.get());
    }
    gap_swaggers_free(swaggers);

    return res;
  }

  // Return a subset of the swaggers.
  State::Users
  State::swaggers(QString const& filter)
  {
    uint32_t* swaggers = gap_swaggers(this->_state);

    State::Users res;
    for (uint32_t i = 0; swaggers[i] != gap_null(); ++i)
    {
      ELLE_ASSERT_CONTAINS(this->_users, swaggers[i]);
      auto const& user = this->_users.at(swaggers[i]);
      if (user->fullname().contains(filter) ||
          user->handle().contains(filter))
        res.append(user.get());
    }
    gap_swaggers_free(swaggers);

    return res;
  }

  State::Users
  State::search(QString const& filter)
  {
    this->_search_future.cancel();
    this->_search_future = QtConcurrent::run(
      [&,filter] {
        std::string text = filter.toStdString();
        if (filter.count('@') == 1 && email_checker.exactMatch(filter))
          return std::vector<uint32_t>{gap_user_by_email(this->_state, text.c_str())};
        else
          return gap_users_search(this->_state, text.c_str());
      });

    this->_search_watcher.setFuture(this->_search_future);

    return this->swaggers(filter);
  }

  // Cancel the search operation.
  void
  State::cancel_search()
  {
    this->_search_future.cancel();
    this->_search_future = FutureSearchResult();
    this->_search_watcher.setFuture(this->_search_future);
  }

  State::Users
  State::results()
  {
    State::Users res;
    for (uint32_t id: this->_last_results)
    {
      if (id != gap_null())
      {
        this->_users[id].reset(new UserModel(this->_state, id));
        res.append(this->_users[id].get());
      }
    }
    return res;
  }

  void
  State::_on_results_ready()
  {
    this->_last_results.clear();
    auto const& future = this->_search_watcher.future();
    if (future.constBegin() != future.constEnd())
    {
      this->_last_results = this->_search_watcher.result();
    }
    else
    {
      ELLE_DEBUG("future empty");
    }
    emit results_ready();
  }

}
