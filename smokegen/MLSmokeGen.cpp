#include "MLSmokeGen.h"
#include "ClassInfo.h"
#include "MLName.h"

#include <algorithm>
#include <stdexcept>


static void
genModule (char const *targetDir, char const *smokeName, char const *moduleName)
{
  char fileName[64];
  snprintf (fileName, sizeof fileName, "%s/%s.ml",
            targetDir, moduleName);

  FILE *fh = fopen (fileName, "w");
  if (!fh)
    throw std::runtime_error (fileName);

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
  char const *conv;
};

static CamlType
camlType (TypeInfo const &type, char const *className)
{
  if (!type.name)
    return { "Unit", "unit", "unit" };

  if (type.kind == TypeInfo::Enum)
    return { "Int", "int", "int" };

  if (type.klass && type.klass->isQObject)
    {
      if (!strcmp (className, "QObject"))
        return { "Object", "methods", "object" };
      return { "Object", "QObject.methods", "object" };
    }

  static cstring_map<CamlType> const map = {
    { "bool", { "Bool", "bool", "bool" } },
    { "char", { "Char", "char", "char" } },
    { "const bool", { "Bool", "bool", "bool" } },
    { "const char*", { "String", "string", "string" } },
    { "double", { "Float", "float", "float" } },
    { "int", { "Int", "int", "int" } },
    { "unsigned int", { "Int", "int", "int" } },
    { "unsigned long", { "Int", "int", "int" } },
    { "long long", { "Int64", "int64", "int64" } },

    // default to unit
    { "bool*", { } },
    { "bool(*)(QIODevice&,const QMap<QString,QVariant>&)", { } },
    { "bool(*)(QIODevice&,QMap<QString,QVariant>&)", { } },
    { "bool(*)(void*)", { } },
    { "bool(*)(void*,long*)", { } },
    { "char*", { } },
    { "char**", { } },
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
    { "const QItemSelection", { } },
    { "const QItemSelection&", { } },
    { "const QKeySequence&", { } },
    { "const QLineF&", { } },
    { "const QList<int>&", { } },
    { "const QList<QAbstractState*>&", { } },
    { "const QList<QGraphicsItem*>&", { } },
    { "const QList<QKeySequence>&", { } },
    { "const QList<QListWidgetItem*>", { } },
    { "const QList<QModelIndex>&", { } },
    { "const QList<QObject*>&", { } },
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
    { "const QMetaObject*", { } },
    { "const QMetaObject&", { } },
    { "const QMimeData*", { } },
    { "const QModelIndex&", { } },
    { "const QObject*", { } },
    { "const QPainterPath&", { } },
    { "const QPalette&", { } },
    { "const QPen&", { } },
    { "const QPicture*", { } },
    { "const QPicture&", { } },
    { "const QPixmap", { } },
    { "const QPixmap*", { } },
    { "const QPixmap&", { } },
    { "const QPoint&", { } },
    { "const QPointF&", { } },
    { "const QPolygonF&", { } },
    { "const QProcessEnvironment&", { } },
    { "const QRect", { } },
    { "const QRect&", { } },
    { "const QRectF&", { } },
    { "const QRegExp&", { } },
    { "const QRegion&", { } },
    { "const QSize&", { } },
    { "const QSizeF&", { } },
    { "const QStandardItem*", { } },
    { "const QString&", { } },
    { "const QStringList&", { } },
    { "const QStyle*", { } },
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
    { "const QUndoCommand*", { } },
    { "const QUrl&", { } },
    { "const QValidator*", { } },
    { "const QVariant&", { } },
    { "const QVector3D&", { } },
    { "const QVector<QPair<double,QVariant> >&", { } },
    { "const QWidget*", { } },
    { "const QX11Info&", { } },
    { "const void*", { } },
    { "double*", { } },
    { "FILE*", { } },
    { "int*", { } },
    { "int&", { } },
    { "long*", { } },
    { "long long*", { } },
    { "QAbstractAnimation*", { } },
    { "QAbstractButton*", { } },
    { "QAbstractEventDispatcher*", { } },
    { "QAbstractFileEngine*", { } },
    { "QAbstractItemDelegate*", { } },
    { "QAbstractItemModel*", { } },
    { "QAbstractItemView*", { } },
    { "QAbstractProxyModel*", { } },
    { "QAbstractState*", { } },
    { "QAbstractTextDocumentLayout*", { } },
    { "QAbstractTextDocumentLayout::PaintContext", { } },
    { "QAbstractTransition*", { } },
    { "QAbstractUndoItem*", { } },
    { "QAccessibleBridge*", { } },
    { "QAccessibleInterface*", { } },
    { "QAction*", { } },
    { "QActionEvent*", { } },
    { "QActionGroup*", { } },
    { "QAnimationGroup*", { } },
    { "QBrush", { } },
    { "QButtonGroup*", { } },
    { "QByteArray", { } },
    { "QByteArray*", { } },
    { "QByteArray(*)(const QString&)", { } },
    { "QCalendarWidget*", { } },
    { "QChar", { } },
    { "QChildEvent*", { } },
    { "QClipboard*", { } },
    { "QCloseEvent*", { } },
    { "QColor", { } },
    { "QCompleter*", { } },
    { "QContextMenuEvent*", { } },
    { "QCoreApplication*", { } },
    { "QCursor", { } },
    { "QCursor*", { } },
    { "QDataStream&", { } },
    { "QDate", { } },
    { "QDateTime", { } },
    { "QDesktopWidget*", { } },
    { "QDir", { } },
    { "QDockWidget*", { } },
    { "QDragEnterEvent*", { } },
    { "QDragLeaveEvent*", { } },
    { "QDragMoveEvent*", { } },
    { "QDropEvent*", { } },
    { "QEasingCurve", { } },
    { "QErrorMessage*", { } },
    { "QEvent*", { } },
    { "QFile&", { } },
    { "QFileIconProvider*", { } },
    { "QFileInfo", { } },
    { "QFlags<QAbstractItemView::EditTrigger>", { } },
    { "QFlags<QAbstractPrintDialog::PrintDialogOption>", { } },
    { "QFlags<QAbstractSpinBox::StepEnabledFlag>", { } },
    { "QFlags<QColorDialog::ColorDialogOption>", { } },
    { "QFlags<QDateTimeEdit::Section>", { } },
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
    { "QFlags<Qt::ItemFlag>", { } },
    { "QFlags<Qt::KeyboardModifier>", { } },
    { "QFlags<Qt::MatchFlag>", { } },
    { "QFlags<Qt::MouseButton>", { } },
    { "QFlags<Qt::Orientation>", { } },
    { "QFlags<Qt::TextInteractionFlag>", { } },
    { "QFlags<Qt::ToolBarArea>", { } },
    { "QFlags<Qt::WindowState>", { } },
    { "QFlags<Qt::WindowType>", { } },
    { "QFlags<QWizard::WizardOption>", { } },
    { "QFocusEvent*", { } },
    { "QFont", { } },
    { "QFontInfo", { } },
    { "QFontMetrics", { } },
    { "QGraphicsEffect*", { } },
    { "QGraphicsEffectSource*", { } },
    { "QGraphicsEllipseItem*", { } },
    { "QGraphicsItem*", { } },
    { "QGraphicsItem**", { } },
    { "QGraphicsItemGroup*", { } },
    { "QGraphicsLayout*", { } },
    { "QGraphicsLineItem*", { } },
    { "QGraphicsPathItem*", { } },
    { "QGraphicsPixmapItem*", { } },
    { "QGraphicsPolygonItem*", { } },
    { "QGraphicsProxyWidget*", { } },
    { "QGraphicsRectItem*", { } },
    { "QGraphicsScene*", { } },
    { "QGraphicsSceneContextMenuEvent*", { } },
    { "QGraphicsSceneDragDropEvent*", { } },
    { "QGraphicsSceneHelpEvent*", { } },
    { "QGraphicsSceneHoverEvent*", { } },
    { "QGraphicsSceneMouseEvent*", { } },
    { "QGraphicsSceneMoveEvent*", { } },
    { "QGraphicsSceneResizeEvent*", { } },
    { "QGraphicsSceneWheelEvent*", { } },
    { "QGraphicsSimpleTextItem*", { } },
    { "QGraphicsTextItem*", { } },
    { "QGraphicsWidget*", { } },
    { "QHeaderView*", { } },
    { "QHelpEvent*", { } },
    { "QHideEvent*", { } },
    { "QIcon", { } },
    { "QIconEngine*", { } },
    { "QIconEngineV2*", { } },
    { "QImage", { } },
    { "QImageIOHandler*", { } },
    { "QInputContext*", { } },
    { "QInputMethodEvent*", { } },
    { "QIODevice*", { } },
    { "QItemEditorFactory*", { } },
    { "QItemSelection", { } },
    { "QItemSelectionModel*", { } },
    { "QKeyEvent*", { } },
    { "QKeySequence", { } },
    { "QLabel*", { } },
    { "QLayout*", { } },
    { "QLayoutItem*", { } },
    { "QLineEdit*", { } },
    { "QList<int>", { } },
    { "QList<QAbstractAnimation*>", { } },
    { "QList<QAbstractButton*>", { } },
    { "QList<QAbstractState*>", { } },
    { "QList<QAbstractTransition*>", { } },
    { "QList<QAction*>", { } },
    { "QList<QByteArray>", { } },
    { "QList<QDockWidget*>", { } },
    { "QList<QGraphicsItem*>", { } },
    { "QList<QGraphicsView*>", { } },
    { "QList<QGraphicsWidget*>", { } },
    { "QList<QKeySequence>", { } },
    { "QList<QListWidgetItem*>", { } },
    { "QList<QMdiSubWindow*>", { } },
    { "QList<QModelIndex>", { } },
    { "QList<QObject*>", { } },
    { "QList<QPair<double,double> >", { } },
    { "QList<QPair<double,QPointF> >", { } },
    { "QList<QPair<int,int> >", { } },
    { "QList<QStandardItem*>", { } },
    { "QList<QTableWidgetItem*>", { } },
    { "QList<QTableWidgetSelectionRange>", { } },
    { "QList<QTextBlock>", { } },
    { "QList<QTextEdit::ExtraSelection>", { } },
    { "QList<QTextFrame*>", { } },
    { "QList<QTreeWidgetItem*>", { } },
    { "QList<QUndoStack*>", { } },
    { "QList<QUrl>", { } },
    { "QList<QWidget*>", { } },
    { "QListWidgetItem*", { } },
    { "QLocale", { } },
    { "QMap<int,QVariant>", { } },
    { "QMap<QDate,QTextCharFormat>", { } },
    { "QMargins", { } },
    { "QMatrix", { } },
    { "QMatrix4x4*", { } },
    { "QMdiArea*", { } },
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
    { "QPaintEngine*", { } },
    { "QPainter*", { } },
    { "QPainterPath", { } },
    { "QPaintEvent*", { } },
    { "QPalette", { } },
    { "QPauseAnimation*", { } },
    { "QPicture*", { } },
    { "QPixmap", { } },
    { "QPixmap*", { } },
    { "QPoint", { } },
    { "QPoint*", { } },
    { "QPoint&", { } },
    { "QPointF", { } },
    { "QPolygon", { } },
    { "QPolygonF", { } },
    { "QPostEventList*", { } },
    { "QPrinter*", { } },
    { "QProcess*", { } },
    { "QProcessEnvironment", { } },
    { "QProgressBar*", { } },
    { "QPushButton*", { } },
    { "QRect", { } },
    { "QRect*", { } },
    { "QRectF", { } },
    { "QRegExp", { } },
    { "QRegion", { } },
    { "QResizeEvent*", { } },
    { "QScrollBar*", { } },
    { "QSessionManager&", { } },
    { "QSet<QAbstractState*>", { } },
    { "QShowEvent*", { } },
    { "QSignalTransition*", { } },
    { "QSize", { } },
    { "QSizeF", { } },
    { "QSizePolicy", { } },
    { "QSocketNotifier*", { } },
    { "QSpacerItem*", { } },
    { "QSplitter*", { } },
    { "QSplitterHandle*", { } },
    { "QStandardItem*", { } },
    { "QState*", { } },
    { "QStateMachine*", { } },
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
    { "QStyleOptionViewItem", { } },
    { "QStyleOptionViewItem*", { } },
    { "QTabBar*", { } },
    { "QTabletEvent*", { } },
    { "QTableWidgetItem*", { } },
    { "QTemporaryFile*", { } },
    { "QTextBlock", { } },
    { "QTextBlockUserData*", { } },
    { "QTextCharFormat", { } },
    { "QTextCodec*", { } },
    { "QTextCursor", { } },
    { "QTextCursor*", { } },
    { "QTextDocument*", { } },
    { "QTextDocumentPrivate*", { } },
    { "QTextFormat", { } },
    { "QTextFrame*", { } },
    { "QTextFrameFormat", { } },
    { "QTextFrame::iterator", { } },
    { "QTextFrameLayoutData*", { } },
    { "QTextInlineObject", { } },
    { "QTextListFormat", { } },
    { "QTextObject*", { } },
    { "QTextObjectInterface*", { } },
    { "QTextOption", { } },
    { "QTextTableCell", { } },
    { "QTextTableFormat", { } },
    { "QThread*", { } },
    { "QTime", { } },
    { "QTimeLine*", { } },
    { "QTimerEvent*", { } },
    { "QToolBar*", { } },
    { "QTransform", { } },
    { "QTranslator*", { } },
    { "QTreeWidgetItem*", { } },
    { "QUndoCommand*", { } },
    { "QUndoGroup*", { } },
    { "QUndoStack*", { } },
    { "QUrl", { } },
    { "QVariant", { } },
    { "QVector3D", { } },
    { "QVector<QPair<double,QVariant> >", { } },
    { "QVector<QTextFormat>", { } },
    { "QWheelEvent*", { } },
    { "QWidget*", { } },
    { "QWindowSurface*", { } },
    { "QWizard*", { } },
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

  return { "Unit", "unit", "unit" };
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
          fprintf (fh, " (arg%d : %s)", argc, camlType (type, method.klass.className).type);
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
      fprintf (fh, "StackItem.of_%s arg%d", camlType (type, method.klass.className).conv, argc);
      argc++;
    }

  fprintf (fh, "]");
}


static void
printInstanceMethodType (FILE *fh, MethodInfo const &method)
{
  for (TypeInfo const &type : method.params)
    fprintf (fh, "%s -> ", camlType (type, method.klass.className).type);
  fprintf (fh, "%s", camlType (method.returnType, method.klass.className).type);
}


static void
printStaticMethod (FILE *fh, MethodInfo const &method)
{
  if (!method.isStatic || method.isEnum)
    return;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  let %s", name.c_str ());
  printParameters (fh, method);
  fprintf (fh, " = StackItem.to_%s (Object.callMethod metaClass \"%s\"",
           camlType (method.returnType, method.klass.className).conv, method.mungedName);
  printArguments (fh, method);
  fprintf (fh, ")\n");
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
  fprintf (fh, " : %s = Pervasives.raise Object.Not_implemented\n", camlType (method.returnType, method.klass.className).type);
}


static void
printInstanceMethod (FILE *fh, MethodInfo const &method)
{
  if (method.isVirtual || method.isStatic || method.isConstructor || method.isDestructor)
    return;

  std::string const &name = mlName (method.mungedName);
  fprintf (fh, "  let %s (self : methods)", name.c_str ());
  printParameters (fh, method);
  fprintf (fh, " = StackItem.to_%s (self#callMethod \"%s\"",
           camlType (method.returnType, method.klass.className).conv, method.mungedName);
  printArguments (fh, method);
  fprintf (fh, ")\n");
}


static void
printStatic (FILE *fh, ClassInfo const &info)
{
  fprintf (fh, "\n");
  fprintf (fh, "\n");

  fprintf (fh, "module Static = struct\n");

  // Inherited statics
  if (info.parents.empty ())
    fprintf (fh, "  (*include Object.Static*)\n");

  for (ClassInfo const *parent : info.parents)
    if (parent->moduleName)
      fprintf (fh, "  (*include %s.Static*)\n", parent->className);

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

  // Non-virtual methods
  if (info.hasNonVirtuals ())
    {
      fprintf (fh, "\n  (* Non-virtual methods *)\n");
      for (MethodInfo const &method : info.methods)
        printInstanceMethod (fh, method);
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
  fprintf (fh, "        this <- self#callMethod metaClass.Object.className []\n");
  fprintf (fh, "    | Some parent ->\n");
  fprintf (fh, "        this <- self#callMethod (metaClass.Object.className ^ \"#\") [parent#this]\n");
  fprintf (fh, "\n");
  fprintf (fh, "end\n");
}


void
genML (char const *targetDir, class_map const &classes)
{
  cstring_map<bool> done;

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
      if (!fh)
        throw std::runtime_error (fileName);

      fprintf (fh, "let metaClass = Object.metaClass %s.smoke \"%s\"\n", info.moduleName, pair.first.c_str ());

      fprintf (fh, "\n");
      fprintf (fh, "\n");

      printMethods (fh, info);

      printStatic (fh, info);

      printClass (fh, info);

      fclose (fh);
    }
}
