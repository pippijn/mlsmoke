static constexpr value Val_none = Val_int (0);

static value
Val_some (value v)
{
  CAMLparam1 (v);
  CAMLlocal1 (some);
  some = caml_alloc (1, 0);
  Store_field (some, 0, v);
  CAMLreturn (some);
}

static value
Some_val (value v)
{
  return Field (v, 0);
}
