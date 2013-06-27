type 'a t
type 'a data

external create : int -> 'a t = "ml_QList_create"
external append : 'a t -> 'a -> unit = "ml_QList_append"
external at : 'a t -> int -> 'a = "ml_QList_get"
external set : 'a t -> int -> 'a -> unit = "ml_QList_set"
external data : 'a t -> 'a data = "ml_QList_data"
external length : 'a t -> int = "ml_QList_length"


let of_array arr =
  let lst = create (Array.length arr) in
  Array.iter (append lst) arr;
  lst
