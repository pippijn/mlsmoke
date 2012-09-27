#pragma once

extern "C" {
#include <caml/mlvalues.h>
}

#include <smoke.h>

enum DataType
{
  // Variants without arguments
  Type_Unit = -1,

  // Variants with arguments
  Type_Bool = 0,
  Type_Char,
  Type_Int,
  Type_IntRef,
  Type_Float,
  Type_Int64,
  Type_String,
  Type_VoidP,
  Type_ClassP,
  Type_ObjectP
};

struct Marshaller
{
  Smoke *const smoke;

  Marshaller (Smoke *smoke)
    : smoke (smoke)
  {
  }

  value toCaml (Smoke::Index typeIndex, Smoke::StackItem const &item) const;
  Smoke::StackItem toSmoke (Smoke::Index typeIndex, value val) const;

  static void checkType (value val, DataType type);
  static DataType dataType (value value);
};
