#include "CamlSmokeBinding.h"
#include "MLName.h"
#include "Marshaller.h"

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <set>
#include <vector>

#include "ml++.h"


#define TRACE_CALLBACK 0


static std::map<Smoke *, CamlSmokeBinding> bindings;


static bool
operator < (Smoke::MethodMap const &lhs, Smoke::MethodMap const &rhs)
{
  return lhs.classId < rhs.classId || lhs.name < rhs.name || lhs.method < rhs.method;
}


void
CamlSmokeBinding::debug (char const *fmt, ...)
{
  printf ("[%s] ", smoke->moduleName ());
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  printf ("\n");
}

void
CamlSmokeBinding::warn (char const *fmt, ...)
{
  printf ("[%s] Warning: ", smoke->moduleName ());
  va_list ap;
  va_start (ap, fmt);
  vprintf (fmt, ap);
  va_end (ap);
  printf ("\n");
}


CamlSmokeBinding::CamlSmokeBinding (Smoke *s)
  : SmokeBinding (s)
{
  for (Smoke::Index i = 0; i < smoke->numMethodMaps; i++)
    {
      Smoke::MethodMap const &map = smoke->methodMaps[i];
      methodMap.insert (std::make_pair (std::make_pair (map.classId, map.method), map.name));
    }
}

CamlSmokeBinding::~CamlSmokeBinding ()
{
  for (auto link : links)
    printf ("Leaked %p (%p)\n", link.first, link.second);
}


void
CamlSmokeBinding::link (void *object, value self)
{
  links[object] = self;
}


void
CamlSmokeBinding::deleted (Smoke::Index classId, void *obj)
{
  auto found = links.find (obj);
  value self = found->second;
  if (self != 0)
    caml_call_method (self, "deleted");
  links.erase (found);
}


std::pair<value, bool>
CamlSmokeBinding::callCamlMethod (value closure, value self, Smoke::Method const &meth, Smoke::Stack args)
{
  Marshaller marshaller (smoke);

  std::vector<value> argv (meth.numArgs + 1);
  argv[0] = self;

  Smoke::Index *types = smoke->argumentList + meth.args;
  for (unsigned char i = 0; i < meth.numArgs; i++)
    argv[i + 1] = marshaller.toCaml (types[i], args[i]);

  value result = caml_callbackN_exn (closure, argv.size (), argv.data ());
  if (Is_exception_result (result))
    {
      value exception = Extract_exception (result);
      if (!strcmp (String_val (Field (Field (exception, 0), 0)), "Object.Not_implemented"))
        return { Val_unit, false };
      throw callback_exception { exception };
    }

  return { result, false };
  //return { result, true };
}


bool
CamlSmokeBinding::callMethod (Smoke::Index method, void *obj,
                              Smoke::Stack args, bool isAbstract)
{
  Smoke::Method meth = smoke->methods[method];

  if (obj != NULL)
    {
      value self = links[obj];
      if (self != 0)
        {
          char const *methodName = smoke->methodNames[meth.name];
#if TRACE_CALLBACK
          debug ("looking up method %s::%s",
                 className (meth.classId), methodName);
#endif
          auto found = methodMap.find ({ meth.classId, method });
          if (found == methodMap.end ())
            {
              return false;
              throw caml_exception (methodName);
            }

          char const *mungedName = smoke->methodNames[found->second];

          std::string const &camlMethodName = mlName (mungedName);
          value closure = caml_get_public_method (self, hash_variant (camlMethodName.c_str ()));
          if (closure != 0)
            {
#if TRACE_CALLBACK
              debug ("calling method %s::%s",
                     className (meth.classId), camlMethodName.c_str ());
#endif
              std::pair<value, bool> result = callCamlMethod (closure, self, meth, args);
#if TRACE_CALLBACK
              debug ("returned %ld", Bool_val (result));
#endif
              return result.second;
            }
          else
            {
              throw caml_exception ("method %s not found in OCaml class %s",
                                    camlMethodName.c_str (), className (meth.classId));
            }
        }
    }

  return false;
}


/*
 * In a bindings runtime, this should return the classname as used
 * in the bindings language, e.g. Qt::Widget in Ruby or
 * Qyoto.QWidget in C#
 */
char *
CamlSmokeBinding::className (Smoke::Index classId)
{
  return (char *)smoke->classes[classId].className;
}


CamlSmokeBinding &
CamlSmokeBinding::binding (Smoke *smoke)
{
  std::map<Smoke *, CamlSmokeBinding>::iterator found = bindings.find (smoke);
  if (found != bindings.end ())
    return found->second;
  return bindings.insert (std::make_pair (smoke, CamlSmokeBinding (smoke))).first->second;
}
