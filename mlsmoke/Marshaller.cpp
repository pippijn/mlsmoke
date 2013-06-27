#include "Marshaller.h"
#include "cstring_container.h"

#include "ml++.h"

static char const *const typeNames[] = {
  "Unit",
  "Bool",
  "Char",
  "Int",
  "IntRef",
  "Float",
  "Int64",
  "String",
  "VoidP",
  "ClassP",
  "ObjectP",
};

static char const *
typeName (DataType dataType)
{
  return typeNames[dataType + 1];
}

DataType
Marshaller::dataType (value value)
{
  if (Is_long (value))
    {
      switch (Int_val (value))
        {
        case 0: return Type_Unit;
        }
    }
  else
    {
      switch (Tag_val (value))
        {
        case  0: return Type_Bool;
        case  1: return Type_Char;
        case  2: return Type_Int;
        case  3: return Type_IntRef;
        case  4: return Type_Float;
        case  5: return Type_Int64;
        case  6: return Type_String;
        case  7: return Type_VoidP;
        case  8: return Type_ClassP;
        case  9: return Type_ObjectP;
        }
    }

  throw caml_exception ("Invalid tag");
}


struct Converter
{
  DataType type;
  value (*toCaml)(Smoke::StackItem item);
  void (*toSmoke)(value val, Smoke::StackItem &item);
};

static cstring_map<Converter> const converters = {
  { "bool",
    { Type_Bool,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_Bool, Val_bool (item.s_bool)); },
      [](value val, Smoke::StackItem &item) { item.s_bool = Bool_val (Field (val, 0)); },
    }
  },
  { "int",
    { Type_Int,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_Int, Val_int (item.s_int)); },
      [](value val, Smoke::StackItem &item) { item.s_int = Int_val (Field (val, 0)); },
    }
  },
  { "int&",
    { Type_IntRef,
      nullptr,
      // XXX: memory leak!
      [](value val, Smoke::StackItem &item) { item.s_voidp = new int (Int_val (Field (Field (val, 0), 0))); },
    }
  },
  { "const char*",
    {
      Type_String,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_String, Val_bp (item.s_voidp)); },
      [](value val, Smoke::StackItem &item) { item.s_voidp = String_val (Field (val, 0)); },
    }
  },
  { "char**",
    {
      Type_VoidP,
      nullptr,
      [](value val, Smoke::StackItem &item) { item.s_voidp = Bp_val (Field (val, 0)); },
    }
  },
  { "QObject*",
    {
      Type_ClassP,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_ClassP, Val_bp (item.s_voidp)); },
      [](value val, Smoke::StackItem &item) { item.s_voidp = Bp_val (Field (val, 0)); },
    }
  },
};


void
Marshaller::checkType (value val, DataType expected, char const *fmt, ...)
{
  char msg[64] = "variant error";
  if (fmt)
    {
      va_list ap;
      va_start (ap, fmt);
      vsnprintf (msg, sizeof msg, fmt, ap);
      va_end (ap);
    }

  DataType camlType = dataType (val);
  if (camlType != expected)
    throw caml_exception ("Expected %s, but got %s (%s)",
                          typeName (expected),
                          typeName (camlType),
                          msg);
}


Converter const *
Marshaller::findConverter (Smoke::Type const &type) const
{
  cstring_map<Converter>::const_iterator found;
  if (type.classId != 0 && Smoke::isDerivedFrom (Smoke::ModuleIndex (smoke, type.classId),
                                                 Smoke::findClass ("QObject")))
    found = converters.find ("QObject*");
  else
    found = converters.find (type.name);

  if (found == converters.end ())
    return nullptr;

  return &found->second;
}


Smoke::StackItem
Marshaller::toSmoke (Smoke::Index typeIndex, value val) const
{
  Smoke::Type const &type = smoke->types[typeIndex];
  if (type.name == nullptr)
    return { nullptr };

  Converter const *converter = findConverter (type);
  if (!converter)
    throw caml_exception ("type %s is not known to Marshaller", type.name);

  if (!converter->toSmoke)
    throw caml_exception ("type %s cannot be converted to Smoke", type.name);

  checkType (val, converter->type);

  Smoke::StackItem item;
  converter->toSmoke (val, item);
  return item;
}


value
Marshaller::toCaml (Smoke::Index typeIndex, Smoke::StackItem const &item) const
{
  Smoke::Type const &type = smoke->types[typeIndex];
  if (type.name == nullptr)
    return Val_unit;

  Converter const *converter = findConverter (type);
  if (!converter)
    {
      //printf ("type %s is not known to Marshaller\n", type.name);
      return Val_unit;
    }

  if (!converter->toCaml)
    throw caml_exception ("type %s cannot be converted to Smoke", type.name);

  return converter->toCaml (item);
}
