module Static = struct
end


type metaClass = {
  className : string;
  klass : Class.t;
  smoke : Smoke.t;
}

let metaClass smoke className =
  let classId =
    match Smoke.findClass className with
    | None -> failwith ("Class " ^ className ^ " not found")
    | Some id -> id
  in
  
  let klass = Smoke.getClass classId in
  let smoke = Smoke.smoke_of_index classId in

  { className; klass; smoke }


let getMethod { className; smoke } methodName =
  Smoke.getMethod (
    match Smoke.findMethod smoke className methodName with
    | None -> failwith ("Method " ^ methodName ^ " not found in class " ^ className)
    | Some id -> id
  )


let callMethod metaClass methodName args retType =
  let meth = getMethod metaClass methodName in
  Smoke.callMethod metaClass.smoke metaClass.klass meth args retType


class virtual methods = object (self)
  method virtual callMethod : string -> StackItem.value list -> StackItem.datatype -> StackItem.value
end

class virtual t metaClass = object (self)

  method virtual this : StackItem.value
  method virtual deleted : unit

  method delete =
    ignore (self#callMethod ("~" ^ metaClass.className) [] StackItem.Type_Unit)

  method callMethod methodName args retType =
    callMethod metaClass methodName (StackItem.of_object self :: args) retType

end
