#include "MethodCall.h"
#include "CamlSmokeBinding.h"

#include "ml++.h"

static const variant<Tag, Tag_Unit                > Unit;
static const variant<Tag, Tag_Int    , Tag_Unknown> Int;
static const variant<Tag, Tag_IntRef , Tag_Unknown> IntRef;
static const variant<Tag, Tag_VoidP  , Tag_Unknown> VoidP;
static const variant<Tag, Tag_ClassP , Tag_Unknown> ClassP;
static const variant<Tag, Tag_ObjectP, Tag_Unknown> ObjectP;


static DataType
dataType (value obj)
{
  int tag = Is_long (obj)
    ? -1 - Int_val (obj)
    : Tag_val (obj);
  switch (tag)
    {
    case Tag_Unit: return Type_Unit;
    case Tag_Bool: return Type_Bool;
    case Tag_Char: return Type_Char;
    case Tag_Int: return Type_Int;
    case Tag_IntRef: return Type_IntRef;
    case Tag_Float: return Type_Float;
    case Tag_Int64: return Type_Int64;
    case Tag_String: return Type_String;
    case Tag_VoidP: return Type_VoidP;
    case Tag_ClassP: return Type_ClassP;
    case Tag_ObjectP: return Type_ObjectP;
    case Tag_Unknown: return Type_Unknown;
    }
  failwith ("Unknown tag");
}


MethodCall::MethodCall (Smoke *smoke, Smoke::Class &klass, Smoke::Method &meth, value arguments, DataType returnType)
  : marshaller (smoke)
  , klass (klass)
  , meth (meth)
  , self (meth.flags & Smoke::mf_static ? 0 : get_self (arguments))
  , object (get_this ())
  , ml_stack (1)
  , smoke_stack (1)
  , types { returnType }
{
  List::iter (arguments, [this](value arg) { add_argument (arg); });

  if (smoke_stack.size () != meth.numArgs + 1)
    failwith ("Incorrect number of arguments.");
}

value
MethodCall::get_self (value &list)
{
  if (list == Val_emptylist)
    failwith ("Argument list needs at least the 'self' pointer.");

  value obj = Field (list, 0);
  list = Field (list, 1);

  return
    match<value> (obj, __func__).with (
       ObjectP, [](value obj) { return obj; }
    );
}

void *
MethodCall::get_this ()
{
  if (self == 0)
    return nullptr;

  value result = caml_call_method (self, "this");

  return
    match<void *> (result, __func__).with (
      Unit,    []()           { return nullptr; },
      ClassP,  [](value obj)  { return (void *)obj; }
    );
}

void
MethodCall::add_argument (value obj)
{
  ml_stack.push_back (obj);
  smoke_stack.push_back (marshaller.toSmoke (obj));
  types.push_back (dataType (obj));

  return;

  Smoke::StackItem &item = smoke_stack.back ();

  match<> (obj, __func__).with (
    Unit,    [&item]()           { item.s_voidp = nullptr; },
    Int,     [&item](value val)  { item.s_int = Int_val (val); },
    IntRef,  [&item](value ref)  { item.s_voidp = new int (Int_val (Field (ref, 0))); },
    VoidP,   [&item](value obj)  { item.s_voidp = (void *)obj; },
    ClassP,  [&item](value obj)  { item.s_voidp = (void *)obj; },
    ObjectP, [&item](value self)
      {
        value result = caml_call_method (self, "this");
        item.s_voidp = (void *)Field (result, 0);
      }
  );
}

void
MethodCall::update_ref (Smoke::StackItem item, value obj, DataType tag)
{
  switch (tag)
    {
    case Type_IntRef:
      {
        int *newVal = static_cast<int *> (item.s_voidp);
        Store_field (Field (obj, 0), 0, Val_int (*newVal));
        //delete newVal;
        break;
      }
    case Type_Unit:
    case Type_Int:
    case Type_VoidP:
    case Type_ClassP:
    case Type_ObjectP:
      break;
    default:
      {
        failwith ("Unknown tag while updating ref argument");
        break;
      }
    }
}

value
MethodCall::return_value () const
{
  static CamlSmokeBinding qtguiBinding (marshaller.smoke);

  Smoke::StackItem const item = smoke_stack.front ();

  if (meth.flags & Smoke::mf_ctor)
    {
      // set the smoke binding
      Smoke::StackItem stack[2];
      stack[1].s_voidp = &qtguiBinding;

      void *object = item.s_voidp;
      klass.classFn (0, object, stack);
      qtguiBinding.link (object, self);
    }

  return marshaller.toCaml (meth.ret, item);
}

value
MethodCall::call ()
{
  klass.classFn (meth.method, object, smoke_stack.data ());

  for (size_t i = 1; i < smoke_stack.size (); i++)
    update_ref (smoke_stack[i], ml_stack[i], types[i]);

  return return_value ();
}
