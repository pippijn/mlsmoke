#include <smoke/qtcore_smoke.h>

#include "CamlSmokeBinding.h"


__BEGIN_DECLS


CAMLprim Smoke *
ml_qtcore_Smoke ()
{
  return qtcore_Smoke;
}

CAMLprim value
ml_init_qtcore_Smoke ()
{
  init_qtcore_Smoke ();
  CamlSmokeBinding::binding (qtcore_Smoke);
  return Val_unit;
}

CAMLprim value
ml_delete_qtcore_Smoke ()
{
  delete_qtcore_Smoke ();
  return Val_unit;
}


__END_DECLS
