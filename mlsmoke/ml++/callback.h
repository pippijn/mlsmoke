#include <cstdarg>

struct caml_exception
{
  caml_exception (char const *fmt, ...)
  {
    va_list ap;
    va_start (ap, fmt);
    vsnprintf (what, sizeof what, fmt, ap);
    va_end (ap);
  }

  char what[128];
};

struct callback_exception
{
  value exn;
};


static void set_arg (int index, value *argv) { }

template<typename T, typename... Args>
static void
set_arg (int index, value *argv, T arg, Args... args)
{
  argv[index] = arg;
  set_arg (index + 1, argv, args...);
}

static value
caml_callback (value closure, size_t narg, value *args)
{
  value result = caml_callbackN_exn (closure, narg, args);
  if (Is_exception_result (result))
    throw callback_exception { Extract_exception (result) };
  return result;
}

template<typename... Args>
static value
caml_call_method (value self, char const *name, Args ...args)
{
  value closure = caml_get_public_method (self, hash_variant (name));
  if (closure == 0)
    throw caml_exception ("Method '%s' not found", name);

  size_t const narg = sizeof... (Args) + 1;
  value argv[narg] = { self };
  set_arg (1, argv, args...);

  return caml_callback (closure, narg, argv);
}
