module Static = struct
end

exception Not_implemented


type metaClass = {
  className : string;
  klass : Class.t;
  smoke : Smoke.t;
}

let metaClass smoke className =
  let classId =
    match Class.find className with
    | None -> failwith ("Class " ^ className ^ " not found")
    | Some id -> id
  in
  
  let klass = Class.get classId in
  let smoke = Smoke.smoke_of_index classId in

  { className; klass; smoke }


let getMethod { className; smoke } methodName =
  Method.get (
    match Method.find smoke className methodName with
    | None -> failwith ("Method " ^ methodName ^ " not found in class " ^ className)
    | Some id -> id
  )


let callMethod metaClass methodName args =
  let meth = getMethod metaClass methodName in
  Smoke.callMethod metaClass.smoke metaClass.klass meth args


class virtual methods = object (self)
  method virtual this : StackItem.value
  method virtual deleted : unit

  method virtual delete : unit
  method virtual callMethod : string -> StackItem.value list -> StackItem.value
end

class virtual t metaClass = object (self)

  method delete =
    StackItem.to_unit (self#callMethod ("~" ^ metaClass.className) [])

  method callMethod methodName args =
    callMethod metaClass methodName (StackItem.of_object self :: args)

end
