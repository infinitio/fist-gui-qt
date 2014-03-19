#ifndef TRANSACTIONMODEL_HXX
# define TRANSACTIONMODEL_HXX

namespace std
{
  template<>
  struct hash<TransactionModel>
  {
    size_t
    operator ()(TransactionModel const& tr) const
    {
      return std::hash<uint32_t>()(tr.id());
    }

    size_t
    operator ()(TransactionModel const& tr)
    {
      return std::hash<uint32_t>()(tr.id());
    }
  };
}

#endif
