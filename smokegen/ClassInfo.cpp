#include "ClassInfo.h"
#include "cstring_container.h"

#include <smoke.h>

#include <cassert>
#include <stdexcept>


typedef std::map<std::pair<Smoke::Index, Smoke::Index>, Smoke::Index> method_map;


std::map<std::string, std::string> const moduleMap = {
  { "phonon", "Phonon" },
  { "qimageblitz", "QImageBlitz" },
  { "qsci", "QSci" },
  { "qt3support", "Qt3Support" },
  { "qtcore", "QtCore" },
  { "qtdbus", "QtDBus" },
  { "qtdeclarative", "QtDeclarative" },
  { "qtgui", "QtGui" },
  { "qthelp", "QtHelp" },
  { "qtnetwork", "QtNetwork" },
  { "qtopengl", "QtOpenGL" },
  { "qtscript", "QtScript" },
  { "qtsql", "QtSql" },
  { "qtsvg", "QtSvg" },
  { "qttest", "QtTest" },
  { "qtuitools", "QtUiTools" },
  { "qtwebkit", "QtWebKit" },
  { "qtxmlpatterns", "QtXmlPatterns" },
  { "qtxml", "QtXml" },
};


template<typename... Args>
static void
warn (char const *message, Args... args)
{
  printf ("%%%% Warning: ");
  printf (message, args...);
  printf ("\n");
}


static void
collectAbstractMethods (std::set<MethodInfo> &abstractMethods, ClassInfo const &info)
{
  for (MethodInfo const &method : info.methods)
    if (method.isPure)
      abstractMethods.insert (method);
  for (ClassInfo const *parent : info.parents)
    collectAbstractMethods (abstractMethods, *parent);
}

static void
removeConcreteMethods (std::set<MethodInfo> &abstractMethods, ClassInfo const &info)
{
  for (MethodInfo const &method : info.methods)
    if (method.isVirtual && !method.isPure)
      abstractMethods.erase (method);
  for (ClassInfo const *parent : info.parents)
    removeConcreteMethods (abstractMethods, *parent);
}

bool
ClassInfo::isAbstract () const
{
  if (hasPures ())
    return true;

  // FIXME: This logic is broken:
  // It should only check whether superclasses implement base virtuals,
  // so if a superclass makes a previously implemented function pure, it
  // is abstract again.
  std::set<MethodInfo> abstractMethods;
  collectAbstractMethods (abstractMethods, *this);
  removeConcreteMethods (abstractMethods, *this);
  return !abstractMethods.empty ();
}

bool
ClassInfo::hasEnums () const
{
  for (MethodInfo const &method : methods)
    if (method.isEnum)
      return true;
  return false;
}

bool
ClassInfo::hasStatics () const
{
  for (MethodInfo const &method : methods)
    if (method.isStatic && !method.isEnum)
      return true;
  return false;
}

bool
ClassInfo::hasPures () const
{
  for (MethodInfo const &method : methods)
    if (method.isPure)
      return true;
  return false;
}

bool
ClassInfo::hasVirtuals () const
{
  for (MethodInfo const &method : methods)
    if (method.isVirtual)
      return true;
  return false;
}

bool
ClassInfo::hasNonVirtuals () const
{
  for (MethodInfo const &method : methods)
    if (!method.isVirtual && !method.isStatic && !method.isConstructor && !method.isDestructor)
      return true;
  return false;
}

void
ClassInfo::addMethod (MethodInfo const &info, char const *className)
{
  if (!methods.insert (info).second)
    // QGlobalSpace functions are in more than one Smoke.
    if (strcmp (className, "QGlobalSpace"))
      warn ("method %s::%s already exists", className, info.mungedName);
}


static TypeInfo
typeInfo (class_map &classes,
          Smoke *smoke,
          Smoke::Index typeIndex)
{
  Smoke::Type const &type = smoke->types[typeIndex];

  TypeInfo typeInfo;
  typeInfo.name = type.name;
  if (type.classId != 0)
    typeInfo.klass = &classes[smoke->classes[type.classId].className];

  typeInfo.isStack     = type.flags & Smoke::tf_stack;
  typeInfo.isPointer   = type.flags & Smoke::tf_ptr;
  typeInfo.isReference = type.flags & Smoke::tf_ref;
  typeInfo.isConst     = type.flags & Smoke::tf_const;
  typeInfo.kind        = TypeInfo::Kind (type.flags & Smoke::tf_elem);

  return typeInfo;
}


static void
collectMethodInfo (class_map &classes,
                   Smoke *smoke,
                   method_map const &methodMap,
                   ClassInfo &classInfo,
                   Smoke::Index methodIndex)
{
  Smoke::Method const &method = smoke->methods[methodIndex];
  Smoke::Class const &klass = smoke->classes[method.classId];

  MethodInfo methodInfo (classInfo);
  methodInfo.name = smoke->methodNames[method.name];
  methodInfo.id = methodIndex;

  methodInfo.isConstructor = method.flags & Smoke::mf_ctor;
  methodInfo.isDestructor  = method.flags & Smoke::mf_dtor;
  methodInfo.isVirtual     = method.flags & Smoke::mf_virtual;
  methodInfo.isPure        = method.flags & Smoke::mf_purevirtual;
  methodInfo.isStatic      = method.flags & Smoke::mf_static;
  methodInfo.isEnum        = method.flags & Smoke::mf_enum;

  assert (smoke->argumentList[method.args + method.numArgs] == 0);
  for (unsigned char arg = 0; arg < method.numArgs; arg++)
    {
      Smoke::Index typeIndex = smoke->argumentList[method.args + arg];

      TypeInfo info = typeInfo (classes, smoke, typeIndex);
      if (!info.name)
        {
          warn ("No type name for type %d", typeIndex);
          continue;
        }
      methodInfo.params.push_back (info);
    }

  methodInfo.returnType = typeInfo (classes, smoke, method.ret);

  method_map::const_iterator found = methodMap.find (std::make_pair (method.classId, methodIndex));
  if (found == methodMap.end ())
    {
      //warn ("no munged name found for method %s::%s", klass.className, methodInfo.name);
      return;
    }

  methodInfo.mungedName = smoke->methodNames[found->second];
  if (!methodInfo.mungedName)
    {
      warn ("munged name for method %s::%s is NULL", klass.className, methodInfo.name);
      return;
    }

  if (methodInfo.isConstructor && !isupper (*methodInfo.name) && strcmp (methodInfo.name, "iterator"))
    {
      warn ("constructor %s::%s does not conform with naming conventions",
            classInfo.className, methodInfo.name);
      return;
    }

  if (methodInfo.isDestructor && *methodInfo.name != '~')
    {
      warn ("method %s::%s does not name a destructor",
            classInfo.className, methodInfo.name);
      return;
    }

  if (methodInfo.isVirtual)
    {
      if (isupper (*methodInfo.name))
        {
          warn ("method %s::%s names a constructor",
                classInfo.className, methodInfo.name);
          return;
        }
      if (*methodInfo.name == '~')
        {
          warn ("method %s::%s names a destructor",
                classInfo.className, methodInfo.name);
          return;
        }
    }

  if (methodInfo.isEnum)
    {
      Smoke::StackItem stack[1];
      klass.classFn (method.method, nullptr, stack);
      methodInfo.enumValue = stack[0].s_int;

#if 0
      long val = methodInfo.enumValue;
      void *ptr = NULL;
      klass.enumFn (Smoke::EnumFromLong, 2, ptr, val);
      printf ("%s::%s = %p\n", klass.className, methodInfo.name, ptr);
      exit (1);
#endif
      //printf ("%s::%s = %d\n", klass.className, methodInfo.name, methodInfo.enumValue);
    }

  classInfo.addMethod (methodInfo, klass.className);
}


static void
collectClassInfo (class_map &classes,
                  Smoke *smoke,
                  char const *moduleName,
                  method_map const &methodMap,
                  Smoke::Index classIndex)
{
  Smoke::Class const &klass = smoke->classes[classIndex];
  if (!klass.className)
    return;

  bool isQObject = Smoke::isDerivedFrom (klass.className, "QObject");
  if (!isQObject)
    return;

  ClassInfo &classInfo = classes[klass.className];
  classInfo.smokeName = smoke->moduleName ();
  classInfo.moduleName = moduleName;
  classInfo.className = klass.className;
  classInfo.isQObject = isQObject;

  // Link parent ClassInfos
  Smoke::Index parents = klass.parents;
  while (Smoke::Index parentIndex = smoke->inheritanceList[parents++])
    classInfo.parents.push_back (&classes[smoke->classes[parentIndex].className]);

  for (Smoke::Index methodIndex = 0; methodIndex < smoke->numMethods; methodIndex++)
    {
      Smoke::Method const &method = smoke->methods[methodIndex];
      if (method.classId == classIndex)
        collectMethodInfo (classes, smoke, methodMap, classInfo, methodIndex);
    }
}


static void
collectClassInfo (class_map &classes, Smoke *smoke)
{
  std::map<std::string, std::string>::const_iterator found = moduleMap.find (smoke->moduleName ());
  if (found == moduleMap.end ())
    throw std::runtime_error ("Module not supported in OCaml.");
  char const *moduleName = found->second.c_str ();

  method_map methodMap;
  for (Smoke::Index i = 0; i < smoke->numMethodMaps; i++)
    {
      Smoke::MethodMap const &map = smoke->methodMaps[i];
      methodMap.insert (std::make_pair (std::make_pair (map.classId, map.method), map.name));
    }

  for (Smoke::Index classIndex = 0; classIndex < smoke->numClasses; classIndex++)
    collectClassInfo (classes, smoke, moduleName, methodMap, classIndex);
}


static void
removeInheritedMethods (ClassInfo &classInfo, std::vector<ClassInfo const *> const &parents)
{
  for (ClassInfo const *parent : parents)
    {
      for (MethodInfo const &method : parent->methods)
        if (method.isStatic)
          classInfo.methods.erase (method);
      removeInheritedMethods (classInfo, parent->parents);
    }
}

class_map
collectClassInfo (std::vector<Smoke *> const &smokes)
{
  class_map classes;

  for (Smoke *smoke : smokes)
    collectClassInfo (classes, smoke);

  for (auto &pair : classes)
    removeInheritedMethods (pair.second, pair.second.parents);

  return classes;
}
