#ifndef FIST_GUI_QT_MODEL_MODEL_HXX
# define FIST_GUI_QT_MODEL_MODEL_HXX

namespace std
{
  template<>
  struct hash<fist::model::Model>
  {
    size_t
    operator ()(fist::model::Model const& tr) const
    {
      return std::hash<uint32_t>()(tr.id());
    }

    size_t
    operator ()(fist::model::Model const& tr)
    {
      return std::hash<uint32_t>()(tr.id());
    }
  };
}

#endif
