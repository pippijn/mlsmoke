extern "C" {
#include <caml/mlvalues.h>
}

#include <vector>

#include "Marshaller.h"

struct MethodCall
{
  Marshaller const marshaller;

  Smoke::Class &klass;
  Smoke::Method &meth;

  value self;
  void *object;
  std::vector<value> ml_stack;
  std::vector<Smoke::StackItem> smoke_stack;
  std::vector<DataType> types;


  MethodCall (Smoke *smoke, Smoke::Class &klass, Smoke::Method &meth, value arguments, DataType returnType);

  value get_self (value &list);
  void *get_this ();

  void add_argument (value obj);
  static void update_ref (Smoke::StackItem item, value obj, DataType tag);
  value return_value () const;

  value call ();
};
