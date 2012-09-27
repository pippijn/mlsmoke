struct caml_exception
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
    throw caml_exception { Extract_exception (result) };
  return result;
}

template<typename... Args>
static value
caml_call_method (value self, char const *name, Args ...args)
{
  value closure = caml_get_public_method (self, hash_variant (name));
  if (closure == 0)
    failwith ("Method not found");

  size_t const narg = sizeof... (Args) + 1;
  value argv[narg] = { self };
  set_arg (1, argv, args...);

  return caml_callback (closure, narg, argv);
}
