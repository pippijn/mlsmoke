#include <cstring>

struct CStringLess
{
  bool operator () (char const *a, char const *b) const
  {
    return a == b ? false : strcmp (a, b) < 0;
  }
};

#include <set>
typedef std::set<char const *, CStringLess> cstring_set;

#include <map>
template<typename Value = char const *>
struct cstring_map
  : std::map<char const *, Value, CStringLess>
{
  typedef std::map<char const *, Value, CStringLess> map;

  cstring_map ()
  {
  }

  cstring_map (std::initializer_list<typename map::value_type> list)
    : map (list)
  {
  }
};
