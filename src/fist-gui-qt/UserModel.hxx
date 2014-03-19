#ifndef USERMODEL_HXX
# define USERMODEL_HXX

namespace std
{
  template<>
  struct hash<UserModel>
  {
    size_t
    operator ()(UserModel const& tr)
    {
      return std::hash<uint32_t>()(tr.id());
    }
  };
}

#endif
