type voidp

type datatype =
  | Type_Unit
  | Type_Bool
  | Type_Char
  | Type_Int
  | Type_IntRef
  | Type_Float
  | Type_Int64
  | Type_String
  | Type_VoidP
  | Type_Class
  | Type_Object

type value =
  | Unit
  | Bool of bool
  | Char of char
  | Int of int
  | IntRef of int ref
  | Float of float
  | Int64 of int64
  | String of string
  | VoidP of voidp
  | Class of voidp
  | Object of < this : value; deleted : unit >

let of_unit () = Unit
let of_bool b = Bool b
let of_char c = Char c
let of_int i = Int i
let of_float f = Float f
let of_int64 i = Int64 i
let of_string s = String s
let of_pointer a = VoidP (Obj.magic a)
let of_object obj = Object (obj :> < this : value; deleted : unit >)

let to_unit   = function Unit     -> () | _ -> failwith "Unit expected"
let to_bool   = function Bool   b -> b  | _ -> failwith "Bool expected"
let to_char   = function Char   c -> c  | _ -> failwith "Char expected"
let to_int    = function Int    i -> i  | _ -> failwith "Int expected"
let to_float  = function Float  f -> f  | _ -> failwith "Float expected"
let to_int64  = function Int64  i -> i  | _ -> failwith "Int64 expected"
let to_string = function String s -> s  | _ -> failwith "String expected"
let to_object = function Object o -> o  | _ -> failwith "Object expected"
