#include "kmainwindow.h"
#include <QTextStream>
#include <QCloseEvent>

KMainWindow::KMainWindow(QWidget *parent, const QString name, Qt::WindowFlags flags, QList<Qt::WidgetAttribute> a):
    QMainWindow(parent, flags) {
  setObjectName(name);
  foreach (Qt::WidgetAttribute attr, a) {
   setAttribute(attr);
  }
  _bar = new KStatusBar(QMainWindow::statusBar());
  _toolBar = new KToolBar();
}

KMainWindow::~KMainWindow() {
  delete _bar;
  delete _toolBar;
}

void KMainWindow::saveMainWindowSettings(KConfig *config, const QString& name) {
  config->setGroup(name);
  QRect geo = geometry();
  config->writeEntry("width", geo.width());
  config->writeEntry("height", geo.height());
  config->writeEntry("maximized", (int)isMaximized());
  config->writeEntry("StatusBar Hidden", (int)statusBar()->isHidden());

  saveToolBar(config, findChild<QToolBar*>("inputBar"));
  saveToolBar(config, findChild<QToolBar*>("QextMdiTaskBar"));
}

void KMainWindow::saveToolBar(KConfig *config, QToolBar *tb) {
  if(tb != NULL) {
    QString name = tb->objectName();
    cout << "saving " << name.toLatin1().data() << endl;
    Qt::ToolBarArea dock;
    int index;
    bool nl;
    int extraOffset;
     //TODO: reimplement of getLocation
    ////getLocation(tb, dock, index, nl, extraOffset);
    QString s;
    QTextStream str(&s, QIODevice::WriteOnly);
    str << (int)dock << " " << index << " " << (int)nl << " " << extraOffset;
    config->writeEntry(name + " Location", s);
    config->writeEntry(name + " Hidden", (int)tb->isHidden());
  }
}

void KMainWindow::applyMainWindowSettings(KConfig *config, const QString& name) {
  config->setGroup(name);
  if(config->readNumEntry("maximized", 0)) {
    resize(QSize(300, 300));
    showMaximized();
  } else {
    QSize s(config->readNumEntry("width", 300),
	    config->readNumEntry("height", 300));
    resize(s);
  }
  if(config->readNumEntry("StatusBar Hidden", 0))
    statusBar()->hide();

  restoreToolBar(config, findChild<QToolBar*>("inputBar"));
  restoreToolBar(config, findChild<QToolBar*>("QextMdiTaskBar"));
}

void KMainWindow::restoreToolBar(KConfig *config, QToolBar *tb) {
  if(tb != NULL) {
    QString name = tb->objectName();
    QString s = config->readEntry(name + " Location", "");
    if(s != "") {
      int dock;
      int index;
      int nl;
      int extraOffset;
      QTextStream str(&s, QIODevice::ReadOnly);
      str >> dock >> index >> nl >> extraOffset;
#warning TODO: reimplement moveToolBar!
      //moveToolBar(tb, (ToolBarDock)dock, (bool)nl, index, extraOffset);
    }
    if(config->readNumEntry(name + " Hidden", 0))
      tb->hide();
  }
}

void KMainWindow::createGUI(QWidget *) {
  QObject *col = actionCollection();
  KAction *inputAction = col->findChild<KAction *>("input_line");

  KAction *fileNew = col->findChild<KAction *>("open_new");
  KAction *fileOpen = col->findChild<KAction *>("open");
  KAction *fileSave = col->findChild<KAction *>("save");
  KAction *fileSaveAs = col->findChild<KAction *>("save_as");
  KAction *fileEdit = col->findChild<KAction *>("file_edit");
  KAction *fileQuit = col->findChild<KAction *>("quit");

  KAction *connect = col->findChild<KAction *>("file_connect");
  KAction *disconnect = col->findChild<KAction *>("file_disconnect");

  KAction *showInputBar = col->findChild<KAction *>("toggle_inputbar");
  KAction *showStatus = col->findChild<KAction *>("toggle_statusbar");
  KAction *showQuickBar = col->findChild<KAction *>("toggle_quickbar");
  KAction *preferences = col->findChild<KAction *>("preferences");


  KActionMenu *scriptingMenu = col->findChild<KActionMenu *>("scripting_menu");

  QMenu *file = new QMenu(this);
  file->setTitle("&File");
  if(fileNew) { fileNew->plug(file, 0); }
  if(fileOpen) { fileOpen->plug(file, 0); }
  if(fileSave) {
      fileSave->plug(file, 0);
      file->insertSeparator(fileSave);
  }
  if(fileSaveAs) { fileSaveAs->plug(file, 0); }

  if(fileEdit) {
      fileEdit->plug(file, 0);
      file->insertSeparator(fileEdit);
  }

  if(fileQuit) {
      fileQuit->plug(file, 0);
      file->insertSeparator(fileQuit);
  }

  QMenu *connection = new QMenu(this);
  connection->setTitle("&Connection");
  if(connect) { connect->plug(connection, 0); }
  if(disconnect) { disconnect->plug(connection, 0); }

  QMenu *options = new QMenu(this);
  options->setTitle("&Settings");
  if(showInputBar) { showInputBar->plug(options, 0); }
  if(showStatus) { showStatus->plug(options, 0); }
  if(showQuickBar) { showQuickBar->plug(options, 0); }
  if(preferences) {
      options->insertSeparator(preferences);
      preferences->plug(options, 0);
  }

  QMenu *help = new QMenu(this);
  help->setTitle("&Help");

  //QMenuBar *menu = new QMenuBar(this);
  QMenuBar * menu = this->menuBar();
  menu->setObjectName("Menu");
  //menu->insertItem("&File", file);
  menu->addMenu(file);
  //menu->insertItem("&Connection", connection);
  menu->addMenu(connection);
  //menu->insertItem("&Settings", options);
  menu->addMenu(options);
  //menu->insertItem("&Help", help);
  menu->addMenu(help);

  if(scriptingMenu) {
    QAction *ta = options->addMenu(scriptingMenu->popupMenu());
    ta->setText("Scripting");
  }


  this->setMenuBar(menu);

  KToolBar *tb = new KToolBar("InputBar", this,
			      Qt::BottomToolBarArea, true,
			      "inputBar");
  if(inputAction) { inputAction->plug(tb,0); }
}

void KMainWindow::closeEvent(QCloseEvent *e) {
  if(queryClose() && queryExit())
    e->accept();
  else
    e->ignore();
}
