install Program ".DEFAULT" [
  (* Target *)
  Name		"smoketest";

  (* Sources *)
  Modules [
    "Test";
  ];

  (* Library dependencies *)
  OCamlRequires [
    "qtsmoke";
  ];
]
