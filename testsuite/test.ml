let _ = QtCore.smoke, QtGui.smoke

let () =
  let app = new QApplication.t in
  let widget = new QWidget.t () in
  let edit = new QTextEdit.t ~parent:widget () in
  let button = new QPushButton.t ~parent:widget () in

  (*QApplication.aboutQt [];*)
  (*QMessageBox.aboutQt [StackItem.Unit];*)

  widget#show;
  QApplication.exec ();

  widget#delete;
  app#delete;

  ()
