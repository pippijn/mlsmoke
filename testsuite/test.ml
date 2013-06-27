let () =
  let app = new QApplication.t in
  let widget = new QWidget.t () in
  let edit = new QTextEdit.t ~parent:widget () in
  let button =
    object (self)
      inherit QPushButton.t ~parent:widget () as super

      method hitButtonO () =
        print_endline "hitButtonO";
        false
    end
  in

  QWidget.show widget;
  QCoreApplication.exec ();

  widget#delete;
  app#delete;

  ()
