#include <QList>


#include "ml++.h"

struct ml_value
{
  ml_value (value data = 0)
    : data (data)
  {
    caml_register_generational_global_root (&data);
  }

  ml_value (ml_value const &rhs)
    : data (rhs.data)
  {
    caml_register_generational_global_root (&data);
  }

  ~ml_value ()
  {
    caml_remove_generational_global_root (&data);
  }

  ml_value &operator = (ml_value const &rhs)
  {
    caml_modify_generational_global_root (&data, rhs.data);
  }


  value data;
};


typedef QList<ml_value> ValueList;

CUSTOM (ValueList);

__BEGIN_DECLS


CAMLprim value
ml_QList_create (value capacity)
{
  ValueList list;
  list.reserve (Int_val (capacity));
  return alloc_object<ValueList> (list);
}


CAMLprim value
ml_QList_append (value list, value element)
{
  object_val<ValueList> (list)->append (element);
  return Val_unit;
}


CAMLprim value
ml_QList_get (value list, value i)
{
  return object_val<ValueList> (list)->at (i).data;
}


CAMLprim value
ml_QList_set (value list, value i, value element)
{
  (*object_val<ValueList> (list))[i] = element;
  return Val_unit;
}


CAMLprim value
ml_QList_data (value list)
{
  return Val_bp (&object_val<ValueList> (list)->front ());
}


CAMLprim value
ml_QList_length (value list)
{
  return Val_int (object_val<ValueList> (list)->length ());
}


__END_DECLS
