#include <cstdarg>

template<typename Arg0, typename ...Args>
static void
failwith (char const *fmt, Arg0 arg0, Args ...args)
{
  char buf[128];
  snprintf (buf, sizeof buf, fmt, arg0, args...);
  failwith (buf);
}
