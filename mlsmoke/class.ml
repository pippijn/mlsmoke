type t
type id

external find : string -> id ModuleIndex.t option = "ml_Smoke_findClass"
external get : id ModuleIndex.t -> t = "ml_Smoke_getClass"
