#include <smoke/qtgui_smoke.h>

#include "CamlSmokeBinding.h"


__BEGIN_DECLS


CAMLprim Smoke *
ml_qtgui_Smoke ()
{
  return qtgui_Smoke;
}

CAMLprim value
ml_init_qtgui_Smoke ()
{
  init_qtgui_Smoke ();
  CamlSmokeBinding::binding (qtgui_Smoke);
  return Val_unit;
}

CAMLprim value
ml_delete_qtgui_Smoke ()
{
  delete_qtgui_Smoke ();
  return Val_unit;
}


__END_DECLS
