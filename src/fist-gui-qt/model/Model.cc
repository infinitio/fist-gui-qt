#include <fist-gui-qt/model/Model.hh>

namespace fist
{
  namespace model
  {
    Model::Model(fist::State& state,
                 ID id)
      : _state(state)
      , _id(id)
    {}

    /*----------.
      | Printable |
      `----------*/
    void
    Model::print(std::ostream& stream) const
    {
      stream << "Model(" << this->_id << ")";
    }
  }
}
