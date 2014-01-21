#ifndef UTILS_HH
# define UTILS_HH

# define Q_PROPERTY_R(Type, Name, Read)                 \
  public:                                               \
    Type                                                \
    Read() const                                        \
    {                                                   \
      return this->_##Name;                             \
    }                                                   \
                                                        \
  private:                                              \
    Type _##Name;                                       \

inline
std::ostream&
operator << (std::ostream& out, QRect const& r)
{
  out << "("
      << r.x() << ", "
      << r.y() << ", "
      << r.width() << ", "
      << r.height()
      << ")";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, QPoint const& p)
{
  out << "("
      << p.x() << ", "
      << p.y()
      << ")";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, QSize const& p)
{
  out << "("
      << p.width() << ", "
      << p.height()
      << ")";
  return out;
}

#endif
