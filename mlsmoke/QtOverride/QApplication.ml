class t = object (self)
  inherit Object.t metaClass
  inherit methods

  val mutable this = StackItem.Unit

  method this = this
  method deleted = this <- StackItem.Unit

  initializer
    let argv = QList.of_array Sys.argv in
    let stack =
      let open StackItem in
      [IntRef (ref (QList.length argv)); of_pointer (QList.data argv)]
    in
    this <- self#callMethod (metaClass.Object.className ^ "$?") stack StackItem.Type_ClassP

end
