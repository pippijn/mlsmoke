install Program ".DEFAULT" [
  (* Target *)
  Name		"smokegen";

  Sources [
    "ClassInfo.cpp";
    "MLSmokeGen.cpp";
    "mlsmokegen.cpp";
  ];

  Headers [
    "ClassInfo.h";
    "MLName.h";
    "MLSmokeGen.h";
    "cstring_container.h";
  ];

  CRequires ["$(SMOKES)"];
]
