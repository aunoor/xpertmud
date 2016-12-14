#include "kmainwindow.h"
#include <qobjcoll.h>
#include <qtextstream.h>

KMainWindow::KMainWindow(QWidget *parent, const char *name, int flags):
  QMainWindow(parent, name) {
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

  saveToolBar(config, (QToolBar*)child("inputBar"));
  saveToolBar(config, (QToolBar*)child("QextMdiTaskBar"));
}

void KMainWindow::saveToolBar(KConfig *config, QToolBar *tb) {
  if(tb != NULL) {
    QString name = tb->name();
    cout << "saving " << name.latin1() << endl;
    ToolBarDock dock;
    int index;
    bool nl;
    int extraOffset;
    getLocation(tb, dock, index, nl, extraOffset);
    QString s;
    QTextStream str(&s, IO_WriteOnly);
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

  restoreToolBar(config, (QToolBar*)child("inputBar"));
  restoreToolBar(config, (QToolBar*)child("QextMdiTaskBar"));
}

void KMainWindow::restoreToolBar(KConfig *config, QToolBar *tb) {
  if(tb != NULL) {
    QString name = tb->name();
    QString s = config->readEntry(name + " Location", "");
    if(s != "") {
      int dock;
      int index;
      int nl;
      int extraOffset;
      QTextStream str(&s, IO_ReadOnly);
      str >> dock >> index >> nl >> extraOffset;
      moveToolBar(tb, (ToolBarDock)dock, (bool)nl, index, extraOffset);
    }
    if(config->readNumEntry(name + " Hidden", 0))
      tb->hide();
  }
}

void KMainWindow::createGUI(QWidget *) {
  QObject *col = actionCollection();
  KAction *inputAction = (KAction *)col->child("input_line");

  KAction *fileNew = (KAction *)col->child("open_new");
  KAction *fileOpen = (KAction *)col->child("open");
  KAction *fileSave = (KAction *)col->child("save");
  KAction *fileSaveAs = (KAction *)col->child("save_as");
  KAction *fileEdit = (KAction *)col->child("file_edit");
  KAction *fileQuit = (KAction *)col->child("quit");

  KAction *connect = (KAction *)col->child("file_connect");
  KAction *disconnect = (KAction *)col->child("file_disconnect");

  KAction *showInputBar = (KAction *)col->child("toggle_inputbar");
  KAction *showStatus = (KAction *)col->child("toggle_statusbar");
  KAction *showQuickBar = (KAction *)col->child("toggle_quickbar");
  KAction *preferences = (KAction *)col->child("preferences");


  KActionMenu *scriptingMenu = (KActionMenu *)col->child("scripting_menu");

  QPopupMenu *file = new QPopupMenu(this);
  if(fileNew) { fileNew->plug(file, 0); }
  if(fileOpen) { fileOpen->plug(file, 0); }
  file->insertSeparator();
  if(fileSave) { fileSave->plug(file, 0); }
  if(fileSaveAs) { fileSaveAs->plug(file, 0); }
  file->insertSeparator();
  if(fileEdit) { fileEdit->plug(file, 0); }
  file->insertSeparator();
  if(fileQuit) { fileQuit->plug(file, 0); }

  QPopupMenu *connection = new QPopupMenu(this);
  if(connect) { connect->plug(connection, 0); }
  if(disconnect) { disconnect->plug(connection, 0); }

  QPopupMenu *options = new QPopupMenu(this);
  if(showInputBar) { showInputBar->plug(options, 0); }
  if(showStatus) { showStatus->plug(options, 0); }
  if(showQuickBar) { showQuickBar->plug(options, 0); }
  options->insertSeparator();
  if(preferences) { preferences->plug(options, 0); }

  QPopupMenu *help = new QPopupMenu(this);

  QMenuBar *menu = new QMenuBar(this, "Menu");
  menu->insertItem("&File", file);
  menu->insertItem("&Connection", connection);
//  if(scriptingMenu) { menu->insertItem("&Scripting", 
//				       scriptingMenu->popupMenu()); }
  menu->insertItem("&Settings", options);
  menu->insertItem("&Help", help);

  KToolBar *tb = new KToolBar("InputBar", this,
			      QMainWindow::Bottom, true,
			      "inputBar");
  if(inputAction) { inputAction->plug(tb,0); }
}

void KMainWindow::closeEvent(QCloseEvent *e) {
  if(queryClose() && queryExit())
    e->accept();
  else
    e->ignore();
}
