#ifndef FIST_GUI_QT_SET_HH
# define FIST_GUI_QT_SET_HH

# include <unordered_set>

# include <fist-gui-qt/containers/Updatable.hh>

namespace fist
{
  template <typename T>
  class Set
    : public UpdatetableContainer
  {
    typedef std::unordered_set<T> Storage;
  public:
    Set() = default;

    inline bool insert(T const& value)
    {
      auto did_something = this->_storage.insert(value).second;
      if (did_something)
        emit size_changed(this->size());
      return did_something;
    }

    inline bool erase(T const& value)
    {
      auto did_something = this->_storage.erase(value);
      if (did_something)
        emit size_changed(this->size());
      return did_something;
    }

#define FORWARD_AND_RENAME_METHOD_WITH_SPECIFIER(name, new_name, specifier) \
    inline                                                                  \
    decltype(std::declval<Storage>().name())                                \
    new_name() specifier { return this->_storage.name(); }                  \

#define FORWARD_AND_RENAME_CONST_METHOD(name, new_name)                     \
    FORWARD_AND_RENAME_METHOD_WITH_SPECIFIER(name, new_name, const)

#define FORWARD_AND_RENAME_METHOD(name, new_name)                           \
    FORWARD_AND_RENAME_METHOD_WITH_SPECIFIER(name, new_name, )

#define FORWARD_CONST_METHOD(name) \
    FORWARD_AND_RENAME_CONST_METHOD(name, name)

#define FORWARD_METHOD(name) \
    FORWARD_AND_RENAME_METHOD(name, name)

    FORWARD_CONST_METHOD(empty);
    FORWARD_CONST_METHOD(size);
    FORWARD_CONST_METHOD(cbegin);
    FORWARD_METHOD(begin);

  private:
    Storage _storage;
  };
};

#endif
