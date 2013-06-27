type t = ModuleIndex.smoke

external smoke_of_index : 'a ModuleIndex.t -> t = "ml_Smoke_ModuleIndex_smoke"

external callMethod : t -> Class.t -> Method.t -> StackItem.value list -> StackItem.value = "ml_Smoke_callMethod"
