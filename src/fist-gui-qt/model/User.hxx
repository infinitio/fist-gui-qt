#ifndef FIST_GUI_QT_MODEL_USER_HXX
# define FIST_GUI_QT_MODEL_USER_HXX

namespace std
{
  template<>
  struct hash<fist::model::User>
  {
    size_t
    operator ()(fist::model::User const& tr) const
    {
      return std::hash<uint32_t>()(tr.id());
    }

    size_t
    operator ()(fist::model::User const& tr)
    {
      return std::hash<uint32_t>()(tr.id());
    }
  };
}

#endif
