#pragma once

extern "C" {
#include <caml/mlvalues.h>
}

#include <map>

#include "Marshaller.h"

/*
 * This class will intercept all virtual method calls and will get
 * notified when an instance created by smoke gets destroyed.
 */
class CamlSmokeBinding
  : SmokeBinding
{
  typedef std::map<std::pair<Smoke::Index, Smoke::Index>, Smoke::Index> method_map;

  Marshaller const marshaller;
  method_map methodMap;
  std::map<void *, value> links;

  void debug (char const *fmt, ...);
  void warn (char const *fmt, ...);

public:
  CamlSmokeBinding (Smoke *s);
  ~CamlSmokeBinding ();

  void link (void *object, value self);

  void deleted (Smoke::Index classId, void *obj);

  value callCamlMethod (value closure, value self, Smoke::Method const &meth, Smoke::Stack args);
  bool callMethod (Smoke::Index method, void *obj,
                   Smoke::Stack args, bool isAbstract);

  /*
   * In a bindings runtime, this should return the classname as used
   * in the bindings language, e.g. Qt::Widget in Ruby or
   * Qyoto.QWidget in C#
   */
  char *className (Smoke::Index classId);


  static CamlSmokeBinding &binding (Smoke *smoke);
};
