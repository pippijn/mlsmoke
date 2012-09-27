#include "MLSmokeGen.h"
#include "ClassInfo.h"
#include "MLName.h"

#include <algorithm>


static std::string
tolower (std::string &&s)
{
  std::transform (s.begin (), s.end (), s.begin (), static_cast<int (*)(int)> (std::tolower));
  return move (s);
}


static void
genModule (char const *targetDir, char const *smokeName, char const *moduleName)
{
  char fileName[64];
  snprintf (fileName, sizeof fileName, "%s/%s.ml",
            targetDir, moduleName);

  FILE *fh = fopen (fileName, "w");
  if (fh == NULL)
    throw fileName;

  fprintf (fh, "external %s_Smoke : unit -> Smoke.t = \"ml_%s_Smoke\"\n", smokeName, smokeName);
  fprintf (fh, "external init_%s_Smoke : unit -> unit = \"ml_init_%s_Smoke\"\n", smokeName, smokeName);
  fprintf (fh, "external delete_%s_Smoke : unit -> unit = \"ml_delete_%s_Smoke\"\n", smokeName, smokeName);
  fprintf (fh, "\n");
  fprintf (fh, "let smoke =\n");
  fprintf (fh, "  init_%s_Smoke ();\n", smokeName);
  fprintf (fh, "  %s_Smoke ()\n", smokeName);

  fclose (fh);
}


static void
printEnumerator (FILE *fh, MethodInfo const &method)
{
  if (!method.isStatic || !method.isEnum)
    return;

  std::string name = method.name;
  name[0] = tolower (name[0]);
  name = mlName (move (name));

  fprintf (fh, "  let %s = %ld\n", name.c_str (), method.enumValue);
}


struct CamlType
{
  char const *tag;
  char const *type;
  char const *null;
};

static CamlType
camlType (TypeInfo const &type)
{
  if (!type.name)
    return { "Unit", "unit", "()" };

  if (type.kind == TypeInfo::Enum)
    return { "Int", "int", "0" };

  static cstring_map<CamlType> const map = {
    { "bool", { "Bool", "bool", "false" } },
    { "char", { "Char", "char", "'\\0'" } },
    { "const bool", { "Bool", "bool", "false" } },
    { "const char*", { "String", "string", "\"\"" } },
    { "double", { "Float", "float", "0.0" } },
    { "int", { "Int", "int", "0 "} },
    { "unsigned int", { "Int", "int", "0" } },
    { "unsigned long", { "Int", "int", "0" } },
    { "long long", { "Int64", "int64", "Int64.of_int 0" } },

    // default
    { "bool*", { } },
    { "bool(*)(QIODevice&,const QMap<QString,QVariant>&)", { } },
    { "bool(*)(QIODevice&,QMap<QString,QVariant>&)", { } },
    { "bool(*)(void*)", { } },
    { "bool(*)(void*,long*)", { } },
    { "char*", { } },
    { "const QAbstractItemModel*", { } },
    { "const QAbstractTextDocumentLayout::PaintContext&", { } },
    { "const QAction*", { } },
    { "const QBrush&", { } },
    { "const QByteArray&", { } },
    { "const QColor&", { } },
    { "const QCursor&", { } },
    { "const QDate&", { } },
    { "const QDateTime&", { } },
    { "const QDir&", { } },
    { "const QEasingCurve&", { } },
    { "const QEvent*", { } },
    { "const QFont&", { } },
    { "const QFontMetrics&", { } },
    { "const QGraphicsItem*", { } },
    { "const QHash<int,QByteArray>&", { } },
    { "const QIcon&", { } },
    { "const QImage&", { } },
    { "const QInputMethodEvent&", { } },
    { "const QItemSelection&", { } },
    { "const QKeySequence&", { } },
    { "const QLineF&", { } },
    { "const QList<int>&", { } },
    { "const QList<QAbstractState*>&", { } },
    { "const QList<QGraphicsItem*>&", { } },
    { "const QList<QKeySequence>&", { } },
    { "const QList<QListWidgetItem*>", { } },
    { "const QList<QModelIndex>&", { } },
    { "const QList<QRectF>&", { } },
    { "const QList<QStandardItem*>&", { } },
    { "const QList<QTableWidgetItem*>", { } },
    { "const QList<QTextEdit::ExtraSelection>&", { } },
    { "const QList<QTreeWidgetItem*>", { } },
    { "const QList<QTreeWidgetItem*>&", { } },
    { "const QList<QUrl>&", { } },
    { "const QList<QWidget*>&", { } },
    { "const QList<QWizard::WizardButton>&", { } },
    { "const QListWidgetItem*", { } },
    { "const QLocale&", { } },
    { "const QMap<int,QVariant>&", { } },
    { "const QMargins&", { } },
    { "const QMatrix&", { } },
    { "const QMetaMethod&", { } },
    { "const QMimeData*", { } },
    { "const QModelIndex&", { } },
    { "const QObject*", { } },
    { "const QPainterPath&", { } },
    { "const QPalette&", { } },
    { "const QPen&", { } },
    { "const QPicture&", { } },
    { "const QPixmap&", { } },
    { "const QPoint&", { } },
    { "const QPointF&", { } },
    { "const QPolygonF&", { } },
    { "const QProcessEnvironment&", { } },
    { "const QRect&", { } },
    { "const QRectF&", { } },
    { "const QRegExp&", { } },
    { "const QRegion&", { } },
    { "const QSize&", { } },
    { "const QSizeF&", { } },
    { "const QStandardItem*", { } },
    { "const QString&", { } },
    { "const QStringList&", { } },
    { "const QStyleOption*", { } },
    { "const QStyleOptionComplex*", { } },
    { "const QStyleOptionGraphicsItem*", { } },
    { "const QStyleOptionViewItem&", { } },
    { "const QTableWidgetItem*", { } },
    { "const QTableWidgetSelectionRange&", { } },
    { "const QTextBlock&", { } },
    { "const QTextCharFormat&", { } },
    { "const QTextCursor&", { } },
    { "const QTextFormat&", { } },
    { "const QTextFrameFormat&", { } },
    { "const QTextListFormat&", { } },
    { "const QTextOption&", { } },
    { "const QTextTableFormat&", { } },
    { "const QTime&", { } },
    { "const QTransform&", { } },
    { "const QTreeWidgetItem*", { } },
    { "const QUrl&", { } },
    { "const QValidator*", { } },
    { "const QVariant&", { } },
    { "const QVector3D&", { } },
    { "const QVector<QPair<double,QVariant> >&", { } },
    { "const QWidget*", { } },
    { "double*", { } },
    { "FILE*", { } },
    { "int*", { } },
    { "int&", { } },
    { "long*", { } },
    { "long long*", { } },
    { "QAbstractAnimation*", { } },
    { "QAbstractButton*", { } },
    { "QAbstractItemDelegate*", { } },
    { "QAbstractItemModel*", { } },
    { "QAbstractItemView*", { } },
    { "QAbstractProxyModel*", { } },
    { "QAbstractState*", { } },
    { "QAbstractTextDocumentLayout*", { } },
    { "QAbstractTransition*", { } },
    { "QAbstractUndoItem*", { } },
    { "QAccessibleBridge*", { } },
    { "QAccessibleInterface*", { } },
    { "QAction*", { } },
    { "QActionEvent*", { } },
    { "QActionGroup*", { } },
    { "QByteArray*", { } },
    { "QByteArray(*)(const QString&)", { } },
    { "QCalendarWidget*", { } },
    { "QChildEvent*", { } },
    { "QCloseEvent*", { } },
    { "QCompleter*", { } },
    { "QContextMenuEvent*", { } },
    { "QDataStream&", { } },
    { "QDockWidget*", { } },
    { "QDragEnterEvent*", { } },
    { "QDragLeaveEvent*", { } },
    { "QDragMoveEvent*", { } },
    { "QDropEvent*", { } },
    { "QEvent*", { } },
    { "QFile&", { } },
    { "QFileIconProvider*", { } },
    { "QFlags<QAbstractItemView::EditTrigger>", { } },
    { "QFlags<QAbstractPrintDialog::PrintDialogOption>", { } },
    { "QFlags<QColorDialog::ColorDialogOption>", { } },
    { "QFlags<QDialogButtonBox::StandardButton>", { } },
    { "QFlags<QDir::Filter>", { } },
    { "QFlags<QDir::SortFlag>", { } },
    { "QFlags<QDockWidget::DockWidgetFeature>", { } },
    { "QFlags<QEventLoop::ProcessEventsFlag>", { } },
    { "QFlags<QFileDialog::Option>", { } },
    { "QFlags<QFile::FileHandleFlag>", { } },
    { "QFlags<QFile::Permission>", { } },
    { "QFlags<QFontComboBox::FontFilter>", { } },
    { "QFlags<QFontDialog::FontDialogOption>", { } },
    { "QFlags<QGraphicsBlurEffect::BlurHint>", { } },
    { "QFlags<QGraphicsEffect::ChangeFlag>", { } },
    { "QFlags<QGraphicsScene::SceneLayer>", { } },
    { "QFlags<QGraphicsView::CacheModeFlag>", { } },
    { "QFlags<QGraphicsView::OptimizationFlag>", { } },
    { "QFlags<QImageIOPlugin::Capability>", { } },
    { "QFlags<QInputDialog::InputDialogOption>", { } },
    { "QFlags<QIODevice::OpenModeFlag>", { } },
    { "QFlags<QItemSelectionModel::SelectionFlag>", { } },
    { "QFlags<QLibrary::LoadHint>", { } },
    { "QFlags<QMainWindow::DockOption>", { } },
    { "QFlags<QMessageBox::StandardButton>", { } },
    { "QFlags<QPageSetupDialog::PageSetupDialogOption>", { } },
    { "QFlags<QPainter::RenderHint>", { } },
    { "QFlags<QPinchGesture::ChangeFlag>", { } },
    { "QFlags<QSizePolicy::ControlType>", { } },
    { "QFlags<Qt::AlignmentFlag>", { } },
    { "QFlags<Qt::DockWidgetArea>", { } },
    { "QFlags<Qt::DropAction>", { } },
    { "QFlags<QTextDocument::FindFlag>", { } },
    { "QFlags<QTextEdit::AutoFormattingFlag>", { } },
    { "QFlags<Qt::GestureFlag>", { } },
    { "QFlags<Qt::InputMethodHint>", { } },
    { "QFlags<Qt::KeyboardModifier>", { } },
    { "QFlags<Qt::MatchFlag>", { } },
    { "QFlags<Qt::TextInteractionFlag>", { } },
    { "QFlags<Qt::ToolBarArea>", { } },
    { "QFlags<Qt::WindowState>", { } },
    { "QFlags<Qt::WindowType>", { } },
    { "QFlags<QWizard::WizardOption>", { } },
    { "QFocusEvent*", { } },
    { "QGraphicsEffect*", { } },
    { "QGraphicsItem*", { } },
    { "QGraphicsItem**", { } },
    { "QGraphicsItemGroup*", { } },
    { "QGraphicsLayout*", { } },
    { "QGraphicsScene*", { } },
    { "QGraphicsSceneContextMenuEvent*", { } },
    { "QGraphicsSceneDragDropEvent*", { } },
    { "QGraphicsSceneHelpEvent*", { } },
    { "QGraphicsSceneHoverEvent*", { } },
    { "QGraphicsSceneMouseEvent*", { } },
    { "QGraphicsSceneMoveEvent*", { } },
    { "QGraphicsSceneResizeEvent*", { } },
    { "QGraphicsSceneWheelEvent*", { } },
    { "QGraphicsWidget*", { } },
    { "QHeaderView*", { } },
    { "QHelpEvent*", { } },
    { "QHideEvent*", { } },
    { "QIconEngine*", { } },
    { "QIconEngineV2*", { } },
    { "QImageIOHandler*", { } },
    { "QInputContext*", { } },
    { "QInputMethodEvent*", { } },
    { "QIODevice*", { } },
    { "QItemEditorFactory*", { } },
    { "QItemSelectionModel*", { } },
    { "QKeyEvent*", { } },
    { "QLabel*", { } },
    { "QLayout*", { } },
    { "QLayoutItem*", { } },
    { "QLineEdit*", { } },
    { "QList<int>", { } },
    { "QList<QAction*>", { } },
    { "QList<QByteArray>", { } },
    { "QList<QPair<int,int> >", { } },
    { "QListWidgetItem*", { } },
    { "QMatrix4x4*", { } },
    { "QMdiSubWindow*", { } },
    { "QMenu*", { } },
    { "QMenuBar*", { } },
    { "QMetaObject::Call", { } },
    { "QMimeData*", { } },
    { "QModelIndex", { } },
    { "QMouseEvent*", { } },
    { "QMoveEvent*", { } },
    { "QMovie*", { } },
    { "QObject*", { } },
    { "QObjectUserData*", { } },
    { "QPaintDevice*", { } },
    { "QPainter*", { } },
    { "QPaintEvent*", { } },
    { "QPicture*", { } },
    { "QPixmap", { } },
    { "QPoint*", { } },
    { "QPoint&", { } },
    { "QPostEventList*", { } },
    { "QPrinter*", { } },
    { "QProcess*", { } },
    { "QProgressBar*", { } },
    { "QPushButton*", { } },
    { "QRect", { } },
    { "QRect*", { } },
    { "QRectF", { } },
    { "QRegion", { } },
    { "QResizeEvent*", { } },
    { "QScrollBar*", { } },
    { "QSessionManager&", { } },
    { "QShowEvent*", { } },
    { "QSize", { } },
    { "QSizeF", { } },
    { "QSizePolicy", { } },
    { "QSocketNotifier*", { } },
    { "QSpacerItem*", { } },
    { "QStandardItem*", { } },
    { "QStatusBar*", { } },
    { "QString", { } },
    { "QString*", { } },
    { "QString&", { } },
    { "QString(*)(const QByteArray&)", { } },
    { "QStringList", { } },
    { "QStyle*", { } },
    { "QStyle&", { } },
    { "QStyleHintReturn*", { } },
    { "QStyleOption*", { } },
    { "QStyleOptionButton*", { } },
    { "QStyleOptionComboBox*", { } },
    { "QStyleOptionDockWidget*", { } },
    { "QStyleOptionFrame*", { } },
    { "QStyleOptionGroupBox*", { } },
    { "QStyleOptionHeader*", { } },
    { "QStyleOptionMenuItem*", { } },
    { "QStyleOptionProgressBar*", { } },
    { "QStyleOptionRubberBand*", { } },
    { "QStyleOptionSlider*", { } },
    { "QStyleOptionSpinBox*", { } },
    { "QStyleOptionTab*", { } },
    { "QStyleOptionTabWidgetFrame*", { } },
    { "QStyleOptionToolBar*", { } },
    { "QStyleOptionToolButton*", { } },
    { "QStyleOptionViewItem*", { } },
    { "QTabBar*", { } },
    { "QTabletEvent*", { } },
    { "QTableWidgetItem*", { } },
    { "QTextBlockUserData*", { } },
    { "QTextCodec*", { } },
    { "QTextCursor*", { } },
    { "QTextDocument*", { } },
    { "QTextFrame*", { } },
    { "QTextFrameLayoutData*", { } },
    { "QTextInlineObject", { } },
    { "QThread*", { } },
    { "QTimeLine*", { } },
    { "QTimerEvent*", { } },
    { "QToolBar*", { } },
    { "QTranslator*", { } },
    { "QTreeWidgetItem*", { } },
    { "QUndoCommand*", { } },
    { "QUndoGroup*", { } },
    { "QUndoStack*", { } },
    { "QVariant", { } },
    { "QWheelEvent*", { } },
    { "QWidget*", { } },
    { "QWindowSurface*", { } },
    { "QWizardPage*", { } },
    { "unsigned char*", { } },
    { "void*", { } },
    { "void**", { } },
    { "_XEvent*", { } },
  };

  auto found = map.find (type.name);
  if (found == map.end ())
    fprintf (stderr, "Type '%s' not mapped\n", type.name);
  else if (found->second.tag)
    return found->second;

  return { "Unit", "unit", "()" };
}


static void
printParameters (FILE *fh, MethodInfo const &method)
{
  if (method.params.empty ())
    {
      if (method.isStatic)
        fprintf (fh, " ()");
    }
  else
    {
      int argc = 0;
      for (TypeInfo const &type : method.params)
        {
          fprintf (fh, " (arg%d : %s)", argc, camlType (type).type);
          argc++;
        }
    }
}

static void
printArguments (FILE *fh, MethodInfo const &method)
{
  fprintf (fh, " [");

  int argc = 0;
  for (TypeInfo const &type : method.params)
    {
      if (argc != 0)
        fprintf (fh, "; ");
      fprintf (fh, "StackItem.of_%s arg%d", camlType (type).type, argc);
      argc++;
    }

  fprintf (fh, "]");
}


static void
printInstanceMethodType (FILE *fh, MethodInfo const &method)
{
  for (TypeInfo const &type : method.params)
    fprintf (fh, "%s -> ", camlType (type).type);
  fprintf (fh, "%s", camlType (method.returnType).type);
}


static void
printStaticMethod (FILE *fh, MethodInfo const &method)
{
  if (!method.isStatic || method.isEnum)
    return;

  char const *tag = camlType (method.returnType).tag;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  let %s", name.c_str ());
  printParameters (fh, method);
  fprintf (fh, " = StackItem.to_%s (Object.callMethod metaClass \"%s\"",
           tolower (tag).c_str (), method.mungedName);
  printArguments (fh, method);
  fprintf (fh, " StackItem.Type_%s)\n", tag);
}


static void
printPureVirtualMethod (FILE *fh, MethodInfo const &method)
{
  if (!method.isVirtual || !method.isPure)
    return;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  method virtual %s : ", name.c_str ());
  printInstanceMethodType (fh, method);
  fprintf (fh, "\n");
}


static void
printVirtualMethod (FILE *fh, MethodInfo const &method)
{
  if (!method.isVirtual || method.isPure)
    return;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  method %s", name.c_str ());
  printParameters (fh, method);
  fprintf (fh, " = %s\n", camlType (method.returnType).null);
}


static void
printInstanceMethod (FILE *fh, MethodInfo const &method)
{
  if (method.isVirtual || method.isStatic || method.isConstructor || method.isDestructor)
    return;

  char const *tag = camlType (method.returnType).tag;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  method %s", name.c_str ());
  printParameters (fh, method);
  fprintf (fh, " = StackItem.to_%s (self#callMethod \"%s\"",
           tolower (tag).c_str (), method.mungedName);
  printArguments (fh, method);
  fprintf (fh, " StackItem.Type_%s)\n", tag);
}


static void
printStatic (FILE *fh, ClassInfo const &info)
{
  fprintf (fh, "module Static = struct\n");

  // Inherited statics
  if (info.parents.empty ())
    fprintf (fh, "  include Object.Static\n");
  for (ClassInfo const *parent : info.parents)
    if (parent->moduleName)
      fprintf (fh, "  include %s.Static\n", parent->className);

  // Enums
  if (info.hasEnums ())
    {
      fprintf (fh, "\n  (* Enumerators *)\n");
      for (MethodInfo const &method : info.methods)
        printEnumerator (fh, method);
    }

  // Static methods
  if (info.hasStatics ())
    {
      fprintf (fh, "\n  (* Static methods *)\n");
      for (MethodInfo const &method : info.methods)
        printStaticMethod (fh, method);
    }

  fprintf (fh, "end\n");
  fprintf (fh, "\n");
  fprintf (fh, "include Static\n");
}


static void
printMethods (FILE *fh, ClassInfo const &info)
{
  // Instance methods
  fprintf (fh, "class virtual methods = object (self)\n");
  fprintf (fh, "  (* Superclasses *)\n");
  if (info.parents.empty ())
    fprintf (fh, "  inherit Object.methods\n");
  for (ClassInfo const *parent : info.parents)
    if (parent->moduleName)
      fprintf (fh, "  inherit %s.methods\n", parent->className);

  // Pure virtual methods
  if (info.hasPures ())
    {
      fprintf (fh, "\n  (* Pure virtual methods *)\n");
      for (MethodInfo const &method : info.methods)
        printPureVirtualMethod (fh, method);
    }

  // Overridden virtual methods
  if (info.hasVirtuals ())
    {
      fprintf (fh, "\n  (* Overridden virtual methods *)\n");
      for (MethodInfo const &method : info.methods)
        printVirtualMethod (fh, method);
    }

  // Non-virtual methods
  if (info.hasNonVirtuals ())
    {
      fprintf (fh, "\n  (* Non-virtual methods *)\n");
      for (MethodInfo const &method : info.methods)
        printInstanceMethod (fh, method);
    }

  fprintf (fh, "end\n");
}


static void
printClass (FILE *fh, ClassInfo const &info)
{
  if (info.isAbstract ())
    return;

  fprintf (fh, "\n");
  fprintf (fh, "\n");

  fprintf (fh, "class t ?parent () = object (self)\n");
  fprintf (fh, "  inherit Object.t metaClass\n");
  fprintf (fh, "  inherit methods\n");
  fprintf (fh, "\n");
  fprintf (fh, "  val mutable this = StackItem.Unit\n");
  fprintf (fh, "\n");
  fprintf (fh, "  method this = this\n");
  fprintf (fh, "  method deleted = this <- StackItem.Unit\n");
  fprintf (fh, "\n");
  fprintf (fh, "  initializer\n");
  fprintf (fh, "    match parent with\n");
  fprintf (fh, "    | None ->\n");
  fprintf (fh, "        this <- self#callMethod metaClass.Object.className [] StackItem.Type_ClassP\n");
  fprintf (fh, "    | Some parent ->\n");
  fprintf (fh, "        this <- self#callMethod (metaClass.Object.className ^ \"#\") [parent#this] StackItem.Type_ClassP\n");
  fprintf (fh, "\n");
  fprintf (fh, "end\n");
}


void
genML (char const *targetDir, class_map const &classes)
{
  std::map<char const *, bool> done;

  for (auto const &pair : classes)
    {
      ClassInfo const &info = pair.second;
      if (!info.moduleName)
        continue;

      if (!done[info.moduleName])
        {
          genModule (targetDir, info.smokeName, info.moduleName);
          done[info.moduleName] = true;
        }

      char fileName[64] = { 0 };
      snprintf (fileName, sizeof fileName, "%s/%s.ml",
                targetDir, info.className);
      for (char &c : fileName)
        if (c == ':')
          c = '_';

      FILE *fh = fopen (fileName, "w");
      if (fh == NULL)
        throw fileName;

      fprintf (fh, "let metaClass = Object.metaClass %s.smoke \"%s\"\n", info.moduleName, pair.first.c_str ());

      fprintf (fh, "\n");
      fprintf (fh, "\n");

      printStatic (fh, info);

      fprintf (fh, "\n");

      printMethods (fh, info);

      printClass (fh, info);

      fclose (fh);
    }
}
