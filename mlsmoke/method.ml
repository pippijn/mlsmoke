type t
type id

external find : ModuleIndex.smoke -> string -> string -> id ModuleIndex.t option = "ml_Smoke_findMethod"
external get : id ModuleIndex.t -> t = "ml_Smoke_getMethod"
