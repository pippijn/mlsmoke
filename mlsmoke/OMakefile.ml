install Library ".DEFAULT" [
  (* Target *)
  Name		"mlsmoke";
  Description	"OCaml Smoke bindings";
  Version	"0.1";

  (* Sources *)
  Modules [
    "Class";
    "Method";
    "ModuleIndex";
    "Object";
    "Smoke";
    "StackItem";
  ];

  (* C++ Sources *)
  Sources [
    "CamlSmokeBinding.cpp";
    "Marshaller.cpp";
    "ml_qlist.cpp";
    "ml_qtcore_smoke.cpp";
    "ml_qtgui_smoke.cpp";
    "ml_smoke.cpp";
  ];

  (* C++ Headers *)
  Headers [
    "CamlSmokeBinding.h";
    "MLName.h";
    "Marshaller.h";
    "cstring_container.h";
    "ml++/callback.h";
    "ml++/fail.h";
    "ml++/list.h";
    "ml++/match.h";
    "ml++/memory.h";
    "ml++/option.h";
    "ml++.h";
  ];

  CRequires ["$(SMOKES) QtCore"];
]
