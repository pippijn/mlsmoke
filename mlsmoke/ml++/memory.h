template<typename T>
struct caml_allocator
{
  struct object
  {
    static char const *const identifier;

    T data;
  };

  struct array
  {
    static char const *const identifier;

    size_t size;
    T data[];
  };

  template<typename Container>
  static custom_operations *operations ()
  {
    static custom_operations operations = {
      (char *)Container::identifier,
      [](value v) { static_cast<Container *> (Data_custom_val (v))->~Container (); },
    };

    return &operations;
  }

  template<typename... Args>
  static value alloc_object (Args const &...data)
  {
    CAMLparam0 ();
    CAMLlocal1 (v);
    v = caml_alloc_custom (operations<object> (), sizeof (T), 0, 1);
    new (Data_custom_val (v)) object { T (data...) };
    CAMLreturn (v);
  }

  static value alloc_array (size_t size)
  {
    CAMLparam0 ();
    CAMLlocal1 (v);
    v = caml_alloc_custom (operations<array> (), sizeof (T) * size, 0, 1);
    new (Data_custom_val (v)) array { size };
    CAMLreturn (v);
  }
};

#define CUSTOM(T) \
  template<> char const *const caml_allocator<T>::object::identifier = #T;    \
  template<> char const *const caml_allocator<T>::array::identifier = #T"[]"


template<typename T, typename... Args>
static value
alloc_object (Args const &...data)
{
  return caml_allocator<T>::alloc_object (data...);
}

template<typename T>
static value
alloc_array (size_t size)
{
  return caml_allocator<T>::alloc_array (size);
}


template<typename T>
static T *
object_val (value v)
{
  return &static_cast<typename caml_allocator<T>::object *> (Data_custom_val (v))->data;
}

template<typename T>
static T *
array_val (value v)
{
  return static_cast<typename caml_allocator<T>::array *> (Data_custom_val (v))->data;
}


static void
store_fields (value v, int index)
{
}

template<typename T, typename... Args>
static void
store_fields (value v, int index, T field, Args... fields)
{
  Store_field (v, index, field);
  store_fields (v, index + 1, fields...);
}

template<typename Enum, typename... Args>
static value
alloc_variant (Enum tag, Args... fields)
{
  value v = caml_alloc (sizeof... (Args), tag);
  store_fields (v, 0, fields...);
  return v;
}
