#ifndef FIST_GUI_QT_MODEL_TRANSACTION_HXX
# define FIST_GUI_QT_MODEL_TRANSACTION_HXX

# include <functional>

# include <fist-gui-qt/model/Transaction.hh>

namespace std
{
  template<>
  struct hash<fist::model::Transaction>
  {
    size_t
    operator ()(fist::model::Transaction const& tr) const
    {
      return std::hash<uint32_t>()(tr.id());
    }

    size_t
    operator ()(fist::model::Transaction const& tr)
    {
      return std::hash<uint32_t>()(tr.id());
    }
  };
}

#endif
