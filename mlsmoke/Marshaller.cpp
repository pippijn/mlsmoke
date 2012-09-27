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
  "Unknown",
};

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
        case 0: return Type_Unit;
        }
    }

  failwith ("Invalid tag");
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
      [](value val, Smoke::StackItem &item) { item.s_bool = Bool_val (Field (val, 0)); }
    }
  },
  { "int",
    { Type_Int,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_Int, Val_int (item.s_int)); },
      [](value val, Smoke::StackItem &item) { item.s_int = Int_val (Field (val, 0)); }
    }
  },
  { "const char*",
    {
      Type_String,
      [](Smoke::StackItem item) { return alloc_variant<DataType> (Type_String, Val_bp (item.s_voidp)); },
      [](value val, Smoke::StackItem &item) { item.s_voidp = String_val (Field (val, 0)); }
    }
  },
};


void
Marshaller::checkType (value val, DataType expected)
{
  DataType camlType = dataType (val);
  if (camlType != expected)
    failwith ("Expected %s, but got %s",
              typeNames[expected],
              typeNames[camlType]);
}


Smoke::StackItem
Marshaller::toSmoke (Smoke::Index typeIndex, value val) const
{
  Smoke::Type const &type = smoke->types[typeIndex];
  if (type.name == nullptr)
    return { nullptr };

  auto found = converters.find (type.name);
  if (found == converters.end ())
    failwith ("type %s cannot be converted to Smoke\n", type.name);

  Converter const &converter = found->second;

  checkType (val, converter.type);

  Smoke::StackItem item;
  converter.toSmoke (val, item);
  return item;
}

value
Marshaller::toCaml (Smoke::Index typeIndex, Smoke::StackItem const &item) const
{
  Smoke::Type const &type = smoke->types[typeIndex];
  if (type.name == nullptr)
    return Val_unit;

  if (type.classId != 0 && Smoke::isDerivedFrom (Smoke::ModuleIndex (smoke, type.classId),
                                                 Smoke::findClass ("QObject")))
    return alloc_variant<DataType> (Type_ClassP, Val_bp (item.s_voidp));

  auto found = converters.find (type.name);
  if (found == converters.end ())
    {
      printf ("type %s cannot be converted to OCaml\n", type.name);
      return Val_unit;
    }

  return found->second.toCaml (item);
}
