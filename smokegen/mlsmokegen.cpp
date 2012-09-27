#include "MLSmokeGen.h"
#include "ClassInfo.h"

#include <smoke/phonon_smoke.h>
#include <smoke/qimageblitz_smoke.h>
#include <smoke/qsci_smoke.h>
#include <smoke/qt3support_smoke.h>
#include <smoke/qtcore_smoke.h>
#include <smoke/qtdbus_smoke.h>
#include <smoke/qtdeclarative_smoke.h>
#include <smoke/qtgui_smoke.h>
#include <smoke/qthelp_smoke.h>
#include <smoke/qtnetwork_smoke.h>
#include <smoke/qtopengl_smoke.h>
#include <smoke/qtscript_smoke.h>
#include <smoke/qtsql_smoke.h>
#include <smoke/qtsvg_smoke.h>
#include <smoke/qttest_smoke.h>
#include <smoke/qtuitools_smoke.h>
#include <smoke/qtwebkit_smoke.h>
#include <smoke/qtxmlpatterns_smoke.h>
#include <smoke/qtxml_smoke.h>


#include <sys/time.h>

static void
status (char const *msg = 0)
{
  static timeval last;

  if (last.tv_sec == 0)
    gettimeofday (&last, NULL);
  else
    {
      timeval now, diff;
      gettimeofday (&now, NULL);
      timersub (&now, &last, &diff);
      last = now;

      printf (" [%ld.%06lds]\n", diff.tv_sec, diff.tv_usec);
    }

  if (msg)
    printf ("%%%% %s", msg);
  fflush (stdout);
}


int
main (int argc, char *argv[])
{
  status ("Initialising Smoke modules...");
  init_phonon_Smoke ();
  init_qimageblitz_Smoke ();
  init_qsci_Smoke ();
  init_qt3support_Smoke ();
  init_qtcore_Smoke ();
  init_qtdbus_Smoke ();
  init_qtdeclarative_Smoke ();
  init_qtgui_Smoke ();
  init_qthelp_Smoke ();
  init_qtnetwork_Smoke ();
  init_qtopengl_Smoke ();
  init_qtscript_Smoke ();
  init_qtsql_Smoke ();
  init_qtsvg_Smoke ();
  init_qttest_Smoke ();
  init_qtuitools_Smoke ();
  init_qtwebkit_Smoke ();
  init_qtxmlpatterns_Smoke ();
  init_qtxml_Smoke ();

  std::vector<Smoke *> smokes = {
    //phonon_Smoke,
    //qimageblitz_Smoke,
    //qsci_Smoke,
    //qt3support_Smoke,
    qtcore_Smoke,
    //qtdbus_Smoke,
    //qtdeclarative_Smoke,
    qtgui_Smoke,
    //qthelp_Smoke,
    //qtnetwork_Smoke,
    //qtopengl_Smoke,
    //qtscript_Smoke,
    //qtsql_Smoke,
    //qtsvg_Smoke,
    //qttest_Smoke,
    //qtuitools_Smoke,
    //qtwebkit_Smoke,
    //qtxmlpatterns_Smoke,
    //qtxml_Smoke,
  };

  char const *targetDir = argc == 2 ? argv[1] : "classes";

  try
    {
      status ("Collecting class information...");
      class_map const &classInfo = collectClassInfo (smokes);
      status ("Generating OCaml code...");
      genML (targetDir, classInfo);
      status ();
    }
  catch (char const *e)
    {
      puts (e);
      return 1;
    }

  return 0;
}
