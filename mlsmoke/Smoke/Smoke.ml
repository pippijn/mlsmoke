type t

type class_id = ModuleIndex.class_id
type method_id = ModuleIndex.method_id

external smoke_of_index : 'a ModuleIndex.t -> t = "ml_Smoke_ModuleIndex_smoke"

external findClass : string -> class_id ModuleIndex.t option = "ml_Smoke_findClass"
external findMethod : t -> string -> string -> method_id ModuleIndex.t option = "ml_Smoke_findMethod"

external getClass : class_id ModuleIndex.t -> Class.t = "ml_Smoke_getClass"
external getMethod : method_id ModuleIndex.t -> Method.t = "ml_Smoke_getMethod"

external callMethod : t -> Class.t -> Method.t -> StackItem.value list -> StackItem.datatype -> StackItem.value = "ml_Smoke_callMethod"
