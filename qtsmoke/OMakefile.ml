install Library ".DEFAULT" [
  (* Target *)
  Name		"qtsmoke";
  Description	"OCaml Qt Smoke bindings";
  Version	"0.1";

  (* Sources *)
  Modules [
    "QtCore/QObject";
    "QtCore/QtCore";
    "QtGui/QAbstractAnimation";
    "QtGui/QAbstractButton";
    "QtGui/QAbstractEventDispatcher";
    "QtGui/QAbstractItemDelegate";
    "QtGui/QAbstractItemModel";
    "QtGui/QAbstractItemView";
    "QtGui/QAbstractListModel";
    "QtGui/QAbstractPageSetupDialog";
    "QtGui/QAbstractPrintDialog";
    "QtGui/QAbstractProxyModel";
    "QtGui/QAbstractScrollArea";
    "QtGui/QAbstractSlider";
    "QtGui/QAbstractSpinBox";
    "QtGui/QAbstractState";
    "QtGui/QAbstractTableModel";
    "QtGui/QAbstractTextDocumentLayout";
    "QtGui/QAbstractTransition";
    "QtGui/QAccessibleBridgePlugin";
    "QtGui/QAccessiblePlugin";
    "QtGui/QAction";
    "QtGui/QActionGroup";
    "QtGui/QAnimationGroup";
    "QtGui/QApplication";
    "QtGui/QBoxLayout";
    "QtGui/QBuffer";
    "QtGui/QButtonGroup";
    "QtGui/QCalendarWidget";
    "QtGui/QCheckBox";
    "QtGui/QClipboard";
    "QtGui/QColorDialog";
    "QtGui/QColumnView";
    "QtGui/QComboBox";
    "QtGui/QCommandLinkButton";
    "QtGui/QCommonStyle";
    "QtGui/QCompleter";
    "QtGui/QCoreApplication";
    "QtGui/QDataWidgetMapper";
    "QtGui/QDateEdit";
    "QtGui/QDateTimeEdit";
    "QtGui/QDesktopWidget";
    "QtGui/QDial";
    "QtGui/QDialog";
    "QtGui/QDialogButtonBox";
    "QtGui/QDirModel";
    "QtGui/QDockWidget";
    "QtGui/QDoubleSpinBox";
    "QtGui/QDoubleValidator";
    "QtGui/QDrag";
    "QtGui/QErrorMessage";
    "QtGui/QEventLoop";
    "QtGui/QEventTransition";
    "QtGui/QFile";
    "QtGui/QFileDialog";
    "QtGui/QFileSystemModel";
    "QtGui/QFileSystemWatcher";
    "QtGui/QFinalState";
    "QtGui/QFocusFrame";
    "QtGui/QFontComboBox";
    "QtGui/QFontDialog";
    "QtGui/QFormLayout";
    "QtGui/QFrame";
    "QtGui/QFutureWatcherBase";
    "QtGui/QGesture";
    "QtGui/QGraphicsAnchor";
    "QtGui/QGraphicsBlurEffect";
    "QtGui/QGraphicsColorizeEffect";
    "QtGui/QGraphicsDropShadowEffect";
    "QtGui/QGraphicsEffect";
    "QtGui/QGraphicsItemAnimation";
    "QtGui/QGraphicsObject";
    "QtGui/QGraphicsOpacityEffect";
    "QtGui/QGraphicsProxyWidget";
    "QtGui/QGraphicsRotation";
    "QtGui/QGraphicsScale";
    "QtGui/QGraphicsScene";
    "QtGui/QGraphicsTextItem";
    "QtGui/QGraphicsTransform";
    "QtGui/QGraphicsView";
    "QtGui/QGraphicsWidget";
    "QtGui/QGridLayout";
    "QtGui/QGroupBox";
    "QtGui/QHBoxLayout";
    "QtGui/QHeaderView";
    "QtGui/QHistoryState";
    "QtGui/QIODevice";
    "QtGui/QIconEnginePlugin";
    "QtGui/QIconEnginePluginV2";
    "QtGui/QImageIOPlugin";
    "QtGui/QInputContext";
    "QtGui/QInputContextPlugin";
    "QtGui/QInputDialog";
    "QtGui/QIntValidator";
    "QtGui/QItemDelegate";
    "QtGui/QItemSelectionModel";
    "QtGui/QKeyEventTransition";
    "QtGui/QLCDNumber";
    "QtGui/QLabel";
    "QtGui/QLayout";
    "QtGui/QLibrary";
    "QtGui/QLineEdit";
    "QtGui/QListView";
    "QtGui/QListWidget";
    "QtGui/QMainWindow";
    "QtGui/QMdiArea";
    "QtGui/QMdiSubWindow";
    "QtGui/QMenu";
    "QtGui/QMenuBar";
    "QtGui/QMessageBox";
    "QtGui/QMimeData";
    "QtGui/QMouseEventTransition";
    "QtGui/QMovie";
    "QtGui/QObjectCleanupHandler";
    "QtGui/QPageSetupDialog";
    "QtGui/QPanGesture";
    "QtGui/QParallelAnimationGroup";
    "QtGui/QPauseAnimation";
    "QtGui/QPictureFormatPlugin";
    "QtGui/QPinchGesture";
    "QtGui/QPlainTextDocumentLayout";
    "QtGui/QPlainTextEdit";
    "QtGui/QPluginLoader";
    "QtGui/QPrintDialog";
    "QtGui/QPrintPreviewDialog";
    "QtGui/QPrintPreviewWidget";
    "QtGui/QProcess";
    "QtGui/QProgressBar";
    "QtGui/QProgressDialog";
    "QtGui/QPropertyAnimation";
    "QtGui/QProxyModel";
    "QtGui/QProxyStyle";
    "QtGui/QPushButton";
    "QtGui/QRadioButton";
    "QtGui/QRegExpValidator";
    "QtGui/QRubberBand";
    "QtGui/QScrollArea";
    "QtGui/QScrollBar";
    "QtGui/QSequentialAnimationGroup";
    "QtGui/QSessionManager";
    "QtGui/QSettings";
    "QtGui/QSharedMemory";
    "QtGui/QShortcut";
    "QtGui/QSignalMapper";
    "QtGui/QSignalTransition";
    "QtGui/QSizeGrip";
    "QtGui/QSlider";
    "QtGui/QSocketNotifier";
    "QtGui/QSortFilterProxyModel";
    "QtGui/QSound";
    "QtGui/QSpinBox";
    "QtGui/QSplashScreen";
    "QtGui/QSplitter";
    "QtGui/QSplitterHandle";
    "QtGui/QStackedLayout";
    "QtGui/QStackedWidget";
    "QtGui/QStandardItemModel";
    "QtGui/QState";
    "QtGui/QStateMachine";
    "QtGui/QStatusBar";
    "QtGui/QStringListModel";
    "QtGui/QStyle";
    "QtGui/QStylePlugin";
    "QtGui/QStyledItemDelegate";
    "QtGui/QSwipeGesture";
    "QtGui/QSyntaxHighlighter";
    "QtGui/QSystemTrayIcon";
    "QtGui/QTabBar";
    "QtGui/QTabWidget";
    "QtGui/QTableView";
    "QtGui/QTableWidget";
    "QtGui/QTapAndHoldGesture";
    "QtGui/QTapGesture";
    "QtGui/QTemporaryFile";
    "QtGui/QTextBlockGroup";
    "QtGui/QTextBrowser";
    "QtGui/QTextCodecPlugin";
    "QtGui/QTextDocument";
    "QtGui/QTextEdit";
    "QtGui/QTextFrame";
    "QtGui/QTextList";
    "QtGui/QTextObject";
    "QtGui/QTextTable";
    "QtGui/QTimeEdit";
    "QtGui/QTimeLine";
    "QtGui/QTimer";
    "QtGui/QToolBar";
    "QtGui/QToolBox";
    "QtGui/QToolButton";
    "QtGui/QTranslator";
    "QtGui/QTreeView";
    "QtGui/QTreeWidget";
    "QtGui/QUndoGroup";
    "QtGui/QUndoStack";
    "QtGui/QUndoView";
    "QtGui/QUnixPrintWidget";
    "QtGui/QVBoxLayout";
    "QtGui/QValidator";
    "QtGui/QVariantAnimation";
    "QtGui/QWidget";
    "QtGui/QWidgetAction";
    "QtGui/QWizard";
    "QtGui/QWizardPage";
    "QtGui/QWorkspace";
    "QtGui/QX11EmbedContainer";
    "QtGui/QX11EmbedWidget";
    "QtGui/QtGui";
    "QtStruct/QList";
  ];

  (* Library dependencies *)
  OCamlRequires [
    "mlsmoke";
  ];

  Recurse "$(glob D, *)";
]
