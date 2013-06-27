#pragma once

#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <set>

struct TypeInfo;
struct MethodInfo;
struct ClassInfo;


struct TypeInfo
{
  enum Kind
  {
    Voidp,
    Bool,
    Char,
    Uchar,
    Short,
    Ushort,
    Int,
    Uint,
    Long,
    Ulong,
    Float,
    Double,
    Enum,
    Class,
    Unknown
  };

  char const *name;
  ClassInfo const *klass;
  Kind kind;
  bool isStack     : 1;
  bool isPointer   : 1;
  bool isReference : 1;
  bool isConst     : 1;

  TypeInfo ()
    : name ("<invalid>")
    , klass (nullptr)
    , kind (Unknown)
    , isStack (false)
    , isPointer (false)
    , isReference (false)
    , isConst (false)
  {
  }

  bool operator < (TypeInfo const &rhs) const
  {
    return strcmp (name, rhs.name) < 0;
  }
};

struct MethodInfo
{
  ClassInfo const &klass;
  char const *name;
  char const *mungedName;
  long enumValue;
  std::vector<TypeInfo> params;
  TypeInfo returnType;
  short id;
  bool isConstructor : 1;
  bool isDestructor  : 1;
  bool isVirtual     : 1;
  bool isPure        : 1;
  bool isStatic      : 1;
  bool isEnum        : 1;

  MethodInfo (ClassInfo const &klass)
    : klass (klass)
    , name ("<invalid>")
    , mungedName (nullptr)
    , enumValue (0)
    , id (0)
    , isConstructor (false)
    , isDestructor (false)
    , isVirtual (false)
    , isPure (false)
    , isStatic (false)
    , isEnum (false)
  {
  }

  bool operator < (MethodInfo const &rhs) const
  {
    return strcmp (mungedName, rhs.mungedName) < 0;
  }
};

struct ClassInfo
{
  char const *smokeName;
  char const *moduleName;
  char const *className;
  std::vector<ClassInfo const *> parents;
  std::set<MethodInfo> methods;

  bool isQObject : 1;

  bool isAbstract () const;

  bool hasEnums () const;
  bool hasStatics () const;
  bool hasPures () const;
  bool hasVirtuals () const;
  bool hasNonVirtuals () const;

  void addMethod (MethodInfo const &info, char const *className);
};


class Smoke;
typedef std::map<std::string, ClassInfo> class_map;

extern std::map<std::string, std::string> const moduleMap;

class_map collectClassInfo (std::vector<Smoke *> const &smokes);
