#include <smoke.h>
#include <smoke/qtgui_smoke.h>

#include "ml++.h"

#include <iostream>

#include "CamlSmokeBinding.h"
#include "Marshaller.h"
 

CUSTOM (Smoke::Class);
CUSTOM (Smoke::Method);
CUSTOM (Smoke::ModuleIndex);
CUSTOM (Smoke::StackItem);

__BEGIN_DECLS


CAMLprim Smoke *
ml_Smoke_ModuleIndex_smoke (value index)
{
  return object_val<Smoke::ModuleIndex> (index)->smoke;
}


CAMLprim value
ml_Smoke_findClass (value c)
{
  Smoke::ModuleIndex classId = Smoke::findClass (String_val (c));
  if (classId.smoke != NULL)
    return Val_some (alloc_object<Smoke::ModuleIndex> (classId));
  return Val_none;
}

CAMLprim value
ml_Smoke_getClass (value index)
{
  Smoke::ModuleIndex const &classId = *object_val<Smoke::ModuleIndex> (index);
  return alloc_object<Smoke::Class> (classId.smoke->classes[classId.index]);
}


CAMLprim value
ml_Smoke_findMethod (Smoke *self, value c, value name)
{
  Smoke::ModuleIndex methId = self->findMethod(String_val (c), String_val (name));
  if (methId.smoke != NULL)
    return Val_some (alloc_object<Smoke::ModuleIndex> (methId));
  return Val_none;
}

CAMLprim value
ml_Smoke_getMethod (value index)
{
  Smoke::ModuleIndex const &methId = *object_val<Smoke::ModuleIndex> (index);
  return alloc_object<Smoke::Method> (methId.smoke->methods[methId.smoke->methodMaps[methId.index].method]);
}


CAMLprim value
ml_Smoke_callMethod (Smoke *smoke, value klassValue, value methValue, value arguments, value return_type)
try
{
  Smoke::Class &klass = *object_val<Smoke::Class> (klassValue);
  Smoke::Method &meth = *object_val<Smoke::Method> (methValue);

  char const *className = klass.className;
  char const *methodName = smoke->methodNames[meth.name];

#if 0
  printf ("%s::%s\n", className, methodName);
#endif

  // static methods don't get an OCaml 'self'
  bool const needsSelf = !(meth.flags &  Smoke::mf_static);
  // constructors and statics don't get a C++ 'this'
  bool const needsThis = !(meth.flags & (Smoke::mf_static | Smoke::mf_ctor));

  value self = 0;
  if (needsSelf)
    {
      if (arguments == Val_emptylist)
        throw caml_exception ("method %s::%s needs a 'self' object and %d arguments",
                              className, methodName, meth.numArgs);
      self = Field (arguments, 0);
      arguments = Field (arguments, 1);

      Marshaller::checkType (self, Type_ObjectP, "for 'self' in method %s::%s", className, methodName);
      self = Field (self, 0);
    }

  void *thisptr = nullptr;
  if (needsThis)
    {
      value thisval = caml_call_method (self, "this");
      Marshaller::checkType (thisval, Type_ClassP, "for 'this' in method %s::%s", className, methodName);
      thisptr = Bp_val (Field (thisval, 0));
    }

  // stack contains 1 for return value
  std::vector<Smoke::StackItem> stack (1 + meth.numArgs);

  Marshaller marshaller (smoke);
  Smoke::Index *types = smoke->argumentList + meth.args;
  for (unsigned char i = 0; i < meth.numArgs; i++)
    {
      if (arguments == Val_emptylist)
        throw caml_exception ("method %s::%s needs and %d arguments, but only got %d",
                              className, methodName, meth.numArgs, i);

      value arg = Field (arguments, 0);
      arguments = Field (arguments, 1);

      stack[i + 1] = marshaller.toSmoke (types[i], arg);
    }

  klass.classFn (meth.method, thisptr, stack.data ());

  if (meth.flags & Smoke::mf_ctor)
    {
      CamlSmokeBinding &binding = CamlSmokeBinding::binding (smoke);
      void *object = stack[0].s_voidp;
      binding.link (object, self);

      // set the smoke binding
      Smoke::StackItem stack[2];
      stack[1].s_voidp = &binding;
      klass.classFn (0, object, stack);
    }

  return marshaller.toCaml (meth.ret, stack[0]);
}
catch (callback_exception const &e)
{
  char exn[48];
  snprintf (exn, sizeof exn, "exception in callback: %p", Field (e.exn, 0));
  failwith (exn);
}
#if 1
catch (caml_exception const &e)
{
  failwith (e.what);
}
#endif


__END_DECLS
