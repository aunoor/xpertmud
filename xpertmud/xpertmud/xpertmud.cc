/***************************************************************************
                          xpertmud.cpp  -  description
                             -------------------
    begin                : Tue Jun 19 11:36:10 MEST 2001
    copyright            : (C) 2001,2002 by Ernst Bachmann, Manuel Klimek
    email                : e.bachmann@xebec.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for QT
#include <QDir>
#include <QToolButton>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QShortcut>

// include files for KDE
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kaccel.h>
#include <kglobalaccel.h>
#include <ktoolbar.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
//#include <kpopupmenu.h>
#include <kurl.h>
#include <kfiledialog.h>

#include <qextmdichildview.h>
#include "xpertmud.h"
#include "scripting/Scripting.h"
#include "configdialog.h"
#include "InputAction.h"
#include "TelnetFilter.h"
#include "BookmarkEditor.h"
#include "TextBuffer.h"
#include "TextBufferView.h"

// debugging includes
//

#include "LogToFile.h"

#include <iostream>
using std::cout;
using std::endl;

#include <cassert>


//#ifndef NO_KDE
//# define DEBUG_DLL
//#endif

#ifndef DEBUG_DLL
# include <klibloader.h>
#else

#define private public
#include <klibloader.h>
#undef private 
#include <ltdl.h>
#include <dlfcn.h>

struct lt_dlhandle_struct {
  struct lt_dlhandle_struct   *next;
  lt_dlloader          *loader;         /* dlopening interface */
  lt_dlinfo             info;
  int                   depcount;       /* number of dependencies */
  lt_dlhandle          *deplibs;        /* dependencies */
  lt_module             module;         /* system module handle */
  lt_ptr                system;         /* system specific data */
  void       *caller_data;    /* per caller associated data */
  int                   flags;          /* various boolean stats */
};
class KLibWrapPrivate
{
public:
    KLibWrapPrivate(KLibrary *l, lt_dlhandle h);

    KLibrary *lib;
    enum {UNKNOWN, UNLOAD, DONT_UNLOAD} unload_mode;
    int ref_count;
    lt_dlhandle handle;
    QString name;
    QString filename;
};
class KLibraryUnloader {
public:
  KLibraryUnloader(KLibrary *l) {
    KLibWrapPrivate *p = 
      KLibLoader::self()->m_libs[QFile::encodeName(l->name())];
    
    //    handle = 
    //      *(lt_dlhandle*)((char *)l + sizeof(KLibrary) - sizeof(void *)*3
    //	- sizeof(QPtrList<QObject>));
    handle = p->handle;
    cout << "Got handle " << handle << endl;
  }
  void forceUnload() {
    cout << "Unloading library " << handle << endl;
    cout << "flags " << handle->flags << endl;
    cout << "refcount: " << handle->info.ref_count << endl;
    cout << "depcount: " << handle->depcount << endl;
    if(handle->flags & 0x1) {
      cout << "Resident!!!!" << endl;
      handle->flags = 0;
    } else {
      cout << "Not resident..." << endl;
    }
    int err;
    do {
      err = unloadOnce();
    } while(err == 0);
    err = unloadOnce();
    err = unloadOnce();
    err = unloadOnce();
    err = unloadOnce();
  }

private:
  int unloadOnce() {
    int err = lt_dlclose(handle);
    if(err) {
      cout << err << ": " << lt_dlerror() << endl;
    } else {
      cout << "Library successfully unloaded" << endl;
    }
    return err;
  }
  lt_dlhandle_struct* handle;
};

class KLibLoaderPrivate
{
public:
    QPtrList<KLibWrapPrivate> loaded_stack;
    QPtrList<KLibWrapPrivate> pending_close;
    enum {UNKNOWN, UNLOAD, DONT_UNLOAD} unload_mode;

    QString errorMessage;
};
class KLibLoader2: public KLibLoader {
public:
  static KLibLoader2 *self() {
    if(!ss_self)
      ss_self = new KLibLoader2;
    return ss_self;
  }

  KLibLoaderPrivate *givePrivate() {
    return (KLibLoaderPrivate *)(this+sizeof(this)-4);
  }

  KLibrary *library (const char *libname)
  {
      KLibrary  *myLib = 0;

      myLog->logMsgF ((char*)"Loading Library (%s)\n", libname);
      myLib = KLibLoader::library (libname);

      if (myLib == 0)
          myLog->logMsgF ((char*)"Failed to load (%s)\n", libname);

      return myLib;
  }

  KLibrary *globalLibrary (const char *libname)
  {
      KLibrary  *myLib = 0;

      myLog->logMsgF ((char*)"Loading Global Library (%s)\n", libname);
      myLib = KLibLoader::globalLibrary (libname);

      if (myLib == 0)
          myLog->logMsgF ((char*)"Failed to load (%s)\n", libname);

      return myLib;
  }


private:
  static KLibLoader2 *ss_self;
};
KLibLoader2* KLibLoader2::ss_self;
#define KLibLoader KLibLoader2
#endif // DEBUG_DLL


Xpertmud::Xpertmud(QWidget *parent, const char *name) : 
  QextMdiMainFrm(parent, name),
  currentLib(NULL),
  scriptInterp(NULL),
  connections(this),
  bytesWritten(0),
  bytesReceived(0),
  command_retention(false)
{

  setMinimumSize(800,600);

  config=kapp->config();

  initStatusBar();
  initView();
  initActions();

  colorMap = new QColor[base_color_table_size];
  for (unsigned int i=0; i<base_color_table_size; ++i) {
    colorMap[i]=base_color_table[i];
  }


  readOptions();

  // disable actions at startup
  fileDisconnect->setEnabled(false);

  slotFindLanguages();

  KGlobal::dirs()->addResourceType
    ("bookmarks", "share/apps/xpertmud/bookmarks");

  connect(&bookmark,SIGNAL(titleChanged(const QString &)),
	  this,SLOT(setWindowTitle(const QString &)));

  setWindowTitle(bookmark.getTitle());
}

Xpertmud::~Xpertmud()
{
  if (scriptInterp) {
    qDebug("delete scriptInterp");
    delete scriptInterp;
    scriptInterp = NULL;
  }
  if (!scriptLibName.isEmpty()) {
    // unload lib
    KLibLoader::self()->unloadLibrary(scriptLibName);
  }

  // TODO: Unload plugin libraries

  delete[] colorMap;
}

void Xpertmud::initialize() {
  int stid=XM_TextBufferWindow_initialize();
  assert(stid==0);
    
    //  TextWidget * statusWindow=getChildTextWidget(stid);

  QString heading = i18n("General Status");
  XM_Window_setTitle(stid, heading);
  XM_Window_maximize(stid);

  slotSwitchLanguage(defaultLanguage);
  if (!scriptInterp) {
    if (availableLanguages.count()) {
      defaultLanguage=availableLanguages[0];
      slotSwitchLanguage(defaultLanguage);
    } else {
      defaultLanguage="";
      ///KMessageBox::error(this,i18n("There are no scripting plugins available!\nPlease check your installation"), i18n("Error!"));
    }
  }

  QTimer *t = new QTimer( this );
  connect( t, SIGNAL(timeout()), SLOT(slotTimer()) );
  t->start( 250 );

  kapp->installEventFilter(this);
}

unsigned int Xpertmud::registerWidget(QWidget* widget) {
  registeredWidgets.push_back(widget);
  return registeredWidgets.size()-1;
}

void Xpertmud::unregisterWidget(QWidget* widget) {
  for(unsigned int i=0; i<registeredWidgets.size(); ++i) {
    if(registeredWidgets[i] == widget) {
      registeredWidgets[i] = NULL;
      // may have registered multiple times, no break
    }
  }
}

void Xpertmud::unregisterWidget(unsigned int id) {
  registeredWidgets[id] = NULL;
}

QWidget* Xpertmud::getRegisteredWidget(unsigned int id) {
  return registeredWidgets[id];
}

void Xpertmud::printToStatusWin(const QString & text) {
  TextBufferViewIface * statusWin= getChildTextWidget(0);
  if (statusWin) {
    statusWin->buffer()->print(text);
  }
}

bool Xpertmud::queryExit() {
  saveOptions();
  return true;
}

bool Xpertmud::queryClose() {
  if(!checkReallyDisconnect(i18n("You're about to close the application."))) {
    return false;
  }

  return saveIfModified();
}

void Xpertmud::initStatusBar() {
  statusBar()->insertItem(i18n("Ready."),1,0);

  statusBar()->insertItem(i18n("Sent:"),2,0,true);
  // the static arg(0) makes sense! Only translate once :)
  statusBar()->insertItem(i18n("%1 Bytes").arg(0),3,0,true);
  statusBar()->insertItem(i18n("Rcvd:"),4,0,true);
  statusBar()->insertItem(i18n("%1 Bytes").arg(0),5,0,true);
  statusBar()->insertItem(i18n("No scripting language loaded"),6,0,true);
}

void Xpertmud::initActions() {
  fileNew = KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());

  fileOpen = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());

  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());

  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());

  fileEdit    = new KAction(i18n("&Edit Bookmark..."),0,this,SLOT(slotEditBookmark()),
			    actionCollection(),"file_edit");
  fileConnect = new KAction(i18n("&Connect..."),0,this,SLOT(slotFileConnect()),
			    actionCollection(), "file_connect");
  fileConnect->setToolTip(i18n("Connect to a MUD"));

  fileDisconnect = new KAction(i18n("&Disconnect"),0,this,SLOT(slotDisconnect()),
			    actionCollection(), "file_disconnect");
  fileDisconnect->setToolTip(i18n("Drop current connection"));

  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotLoadBookmark(const KURL&)), 
					 actionCollection(),"file_recent");

  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  fileQuit->setToolTip(i18n("Quits the application"));

  viewToolBar = KStdAction::showToolbar(this, SLOT(slotViewToolBar()), actionCollection(), "toggle_toolbar");
  viewToolBar->setToolTip(i18n("Enables/disables the toolbar"));

  viewInputBar = new KToggleAction(i18n("Show &Inputbar"),0,this, SLOT(slotViewInputBar()), actionCollection(), "toggle_inputbar");
  viewInputBar->setToolTip(i18n("Enables/disables the inputbar"));

  viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection(), "toggle_statusbar");
  viewStatusBar->setToolTip(i18n("Enables/disables the statusbar"));

  // TODO: new KToggleAction
  viewQuickBar = KStdAction::showToolbar(this, SLOT(slotViewQuickBar()), actionCollection(), "toggle_quickbar");
  viewQuickBar->setToolTip(i18n("Enables/disables the MDI bar"));

  configureToolbars = KStdAction::configureToolbars(this, SLOT(slotEditToolbars()), actionCollection(),"edit_toolbars");
  configureToolbars->setToolTip(i18n("Configure the toolbar"));

  preferences = KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection(), "preferences");
  preferences->setToolTip(i18n("Configure the application"));

  windowTile = new KAction(i18n("&Tile"), 0, this, SLOT(slotWindowTile()), actionCollection(),"window_tile");
  windowCascade = new KAction(i18n("&Cascade"), 0, this, SLOT(slotWindowCascade()), actionCollection(),"window_cascade");

  windowMenu = new KActionMenu(i18n("&Window"), actionCollection(), "window_menu");
  connect(windowMenu->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(windowMenuAboutToShow()));

  scriptingMenu = new KActionMenu(i18n("&Scripting"), actionCollection(), "scripting_menu");
  connect(scriptingMenu->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(scriptingMenuAboutToShow()));

  unloadLanguage = new KAction(i18n("&Unload the Interpreter"),0,this,SLOT(slotUnloadLanguage()),
			       actionCollection(), "unload_language");
  unloadLanguage->setToolTip(i18n("Unloads any currently loaded Interpreter"));
  

  inputAction=new InputAction(i18n("Input Line"), 0, this,
			      SLOT(slotTextEntered(const QString&)),
			      SIGNAL(echoMode(bool)),
			      SIGNAL(commandRetention(bool)),
			      actionCollection(), "input_line",
			      this);

  echoModeAction=new KToggleAction(i18n("&Toggle Inputmode"),0,
				   NULL, NULL, 
				   actionCollection(), "toggle_inputmode");

  echoModeAction->setChecked(true);

  connect(this,SIGNAL(echoMode(bool)),
	  echoModeAction, SLOT(setChecked(bool)));

  connect(echoModeAction,SIGNAL(toggled(bool)),
	  this,SLOT(setEchoMode(bool)));

  emit commandRetention(command_retention);
  createGUI(NULL);

  setMenuForSDIModeSysButtons(menuBar());
  
  // No more menu focus by holding ALT
  removeEventFilter(menuBar());
}


void Xpertmud::windowMenuAboutToShow() {
  /*
  windowMenu->popupMenu()->clear();
  //  windowMenu->insert(windowNewWindow);
  windowMenu->insert(windowCascade);
  windowMenu->insert(windowTile);

  if (pWorkspace->windowList().isEmpty()) {
    //    windowNewWindow->setEnabled(false);
    windowCascade->setEnabled(false);
    windowTile->setEnabled(false);
  } else {
    //    windowNewWindow->setEnabled(true);
    windowCascade->setEnabled(true);
    windowTile->setEnabled(true);
  }
  windowMenu->popupMenu()->insertSeparator();

  QWidgetList windows = pWorkspace->windowList();
  for (int i = 0; i < int(windows.count()); ++i) {
    int id = windowMenu->popupMenu()->insertItem(QString("&%1 ").arg(i+1)+windows.at(i)->caption(), this, SLOT( windowMenuActivated( int ) ) );
    windowMenu->popupMenu()->setItemParameter( id, i );
    windowMenu->popupMenu()->setItemChecked( id, pWorkspace->activeWindow() == windows.at(i) );
  }
  */
}

/*
void Xpertmud::windowMenuActivated(int id) {
  QWidget* w = pWorkspace->windowList().at( id );
  if ( w ) {
    w->showNormal();
  }
  }*/


void Xpertmud::scriptingMenuAboutToShow() {
  scriptingMenu->popupMenu()->clear();
  
  scriptingMenu->insert(unloadLanguage);

  scriptingMenu->popupMenu()->addSeparator();
  for (int i = 0; i < (int)availableLanguages.count(); ++i) {
    QAction* id;
    if (availableLanguages[i] == currentLanguage) { 
      id = scriptingMenu->popupMenu()->addAction(i18n("Restart &%1 interpreter").arg(availableLanguages[i]), this,
						  SLOT( slotSwitchLanguage( int ) ));
    } else {
      id = scriptingMenu->popupMenu()->addAction(i18n("Start &%1 interpreter").arg(availableLanguages[i]), this,
						  SLOT( slotSwitchLanguage( int ) ));
    }
    //scriptingMenu->popupMenu()->setItemParameter(id, i);
    id->setProperty("id",i);
  }
}

bool Xpertmud::saveIfModified() {
  if (bookmark.edited()) {
   int want_save = 
     KMessageBox::warningYesNoCancel(this,
				     i18n("The current bookmark has been modified.\n"
					  "Do you want to save it?"),
				     i18n("Warning"));
    switch(want_save) {
    case KMessageBox::Yes:
      slotFileSave();
      return !bookmark.url().isEmpty() && bookmark.url().isLocalFile();
      break;

    case KMessageBox::No:
      return true;
      break;

    default:
      return false;
    }
  }
  return true;
}


void Xpertmud::slotFileNew() {
  if (saveIfModified()) {
    bookmark = Bookmark();
    slotEditBookmark();
  }
}

bool Xpertmud::checkReallyDisconnect(const QString& what) {
  int count = connections.countConnected();
  if (count > 0) {
    if (KMessageBox::warningYesNo(this,
				  i18n("There are %1 open connections. Really close?").arg(count),
				  /* TODO:? i18n("Warning: ")+ */ what,
				  KStdGuiItem::yes(),
				  KStdGuiItem::no(),
				  "close_if_connected")
	== KMessageBox::No) {
      
      return false;
    }
  }
  return true;
}

void Xpertmud::slotFileOpen() {
  if(!checkReallyDisconnect(i18n("You're about to load a new bookmark"))) {
    return;
  }
  slotDisconnect();
  if (saveIfModified()) {

    // TODO: customized dialog...
    KURL url=KFileDialog::getOpenURL( KGlobal::dirs()->saveLocation("bookmarks") /*":bookmark"*/, 
				     i18n("*.xmud|xpertmud bookmark files (*.xmud)\n*|All Files"), 
				     this, i18n("Open File..."));
    if(!url.isEmpty()) {
      slotLoadBookmark(url);
    }
  }
}


void Xpertmud::slotFileSave() {
  if (bookmark.url().isEmpty() || !bookmark.url().isLocalFile()) {
    slotFileSaveAs();
  } else
    slotSaveBookmark(bookmark.url());
}

void Xpertmud::slotFileSaveAs() {
  KURL url=KFileDialog::getSaveURL(KGlobal::dirs()->saveLocation("bookmarks"),
				     i18n("*.xmud|xpertmud bookmark files (*.xmud)\n*|All Files"), 
				     this, i18n("Save as..."));
  if(!url.isEmpty()) {
    QString fn = url.fileName();
    if(fn.indexOf('.') == -1) {
      url.setFileName(fn + ".xmud");
    }
    slotSaveBookmark(url);
  }
}

void Xpertmud::slotFindLanguages() {
  QStringList filenames=KGlobal::dirs()->findAllResources("module","libxm*interpreter.la",false,true);
  availableLanguages.clear();

//  for (QStringList::Iterator it=filenames.begin();
//       it != filenames.end(); ++it) {


  foreach(QString str, filenames){

    QString lang=str;
    lang.remove(str.length()-14, 14);
    lang.remove(0,lang.lastIndexOf("/libxm")+6);
    availableLanguages.append(lang);
  }

  availableLanguages.sort();
}

void Xpertmud::slotSwitchLanguage(int nr) {
  //TODO: parse parametr from QAction!
  if (nr<(int)availableLanguages.count() && nr >= 0) {
    QString lang = availableLanguages[nr];

    // changing availableLanguages in there...
    slotSwitchLanguage(lang);
  }
}

void Xpertmud::slotSwitchLanguage(const QString& langParam) {
  QString lang = langParam;
  if (scriptInterp) {
    qDebug("delete scriptInterp");
    delete scriptInterp;
    scriptInterp = NULL;

    currentLanguage="";
    statusBar()->changeItem(i18n("stopped interpreter"),6);
  }
  //  if (!scriptLibName.isEmpty()) {
  if(currentLib != NULL) {
    // unload lib
#ifdef DEBUG_DLL
    cout << "unloading " << scriptLibName << endl;
    KLibraryUnloader unloader(currentLib);
    unloader.forceUnload();
#endif

    // won't work, but we have to call it, because
    // the library object can only be deleted this way...
    //    currentLib->unload();
    statusBar()->changeItem(i18n("unloaded interpreter"),6);
    scriptLibName="";
    currentLib = NULL;
  }

  if (!lang.isEmpty()) { 
    if(lang == "__DEFAULT__") {
      if(availableLanguages.count() > 0) {
	lang = availableLanguages[0];
	defaultLanguage = lang;
      } else {
	return;
      }
    }

    QString newLibName=QString("libxm%1interpreter").arg(lang);

    // call to globalLibrary to get the exported symbols from
    // the library
    currentLib = KLibLoader::self()->globalLibrary(newLibName);
    KLibFactory* fac = NULL;
    if(currentLib)
      fac = currentLib->factory();

    if (fac) {
      // beware! it could be, that fac is not valid, this is
      // probably some evil KLibLoader bug
      scriptLibName=newLibName;
      QObject * scripting = fac->create(this,"myXMScripting","XMScripting");
      if (scripting && scripting->inherits("XMScripting")) {
	scriptInterp=static_cast<XMScripting*>(scripting);
	qDebug() << QString("create scriptInterp %1").arg((unsigned long)scriptInterp);
      } else {
	if (scripting)
	  delete scripting;
      }
      if (scriptInterp) {
	scriptInterp->setCallback(this);
	statusBar()->changeItem(lang,6);
	currentLanguage=lang;
      } else {
	KMessageBox::error
	  (this,i18n("Library loaded, but no ScriptInterpreter created"), 
	   i18n("Error!"));
      }
    } else {
      KMessageBox::error(this,i18n("Library could not be loaded: \n%1").
			 arg(KLibLoader::self()->lastErrorMessage()),
			 i18n("Error while loading %1!").arg(lang));
    }
  }
}

void Xpertmud::slotUnloadLanguage() {
  slotSwitchLanguage("");
}

void Xpertmud::slotStatusMsg(const QString &text) {
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text,1);
}

void Xpertmud::initView() {
  /*
  ////////////////////////////////////////////////////////////////////
  // here the main view of the KMainWindow is created by a background box and
  // the QWorkspace instance for MDI view.
  QVBox* view_back = new QVBox( this );
  view_back->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
xpertmud.cc
  pWorkspace = new QextMdiMainFrm( view_back );
  pWorkspace = new QWorkspace( view_back,"workspace");
  pWorkspace->setFocusPolicy(NoFocus);
1.140
  pWorkspace->show();
  setCentralWidget(view_back);
  */
}


void Xpertmud::slotViewToolBar() {
  slotStatusMsg(i18n("Toggle the toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
    toolBar("mainToolBar")->hide();
  else
    toolBar("mainToolBar")->show();

  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotViewInputBar() {
  slotStatusMsg(i18n("Toggle the inputbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewInputBar->isChecked())
    toolBar("inputBar")->hide();
  else
    toolBar("inputBar")->show();

  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotViewStatusBar() {
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Statusbar on or off
  if(!viewStatusBar->isChecked())
    statusBar()->hide();
  else
    statusBar()->show();

  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotViewQuickBar() {
  slotStatusMsg(i18n("Toggle the task bar..."));
  if(!viewQuickBar->isChecked())
    toolBar("QextMdiTaskBar")->hide();
  else
    toolBar("QextMdiTaskBar")->show();

  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotWindowTile() {
  //pWorkspace->tile();
}

void Xpertmud::slotWindowCascade() {
  //pWorkspace->cascade();
}

void Xpertmud::slotEditToolbars() {
  saveMainWindowSettings( KGlobal::config(), "MainWindow" );
  
  KEditToolbar dlg(actionCollection());
  connect(&dlg,SIGNAL(newToolbarConfig()),this,
	  SLOT(slotNewToolbarConfig()));
  if (dlg.exec())
  {
    createGUI(NULL);
  }
}

void Xpertmud::slotConfigure() {
  ConfigDialog cfg(this);
  cfg.setColors(colorMap);
  cfg.setHistSize(scrollbacksize);
  cfg.setScriptingLangs(availableLanguages);
  cfg.setDefaultLanguage(defaultLanguage);
  cfg.setDefaultFont(defaultFont);
  cfg.setCommandRetention(command_retention);
  if (cfg.exec()) {
    cfg.storeColors(colorMap);
    // redraw all text(buffer)widgets
    emit colorConfigChanged();
    scrollbacksize=cfg.getHistSize();
    emit historyConfigChanged((unsigned int)scrollbacksize);
    defaultLanguage=cfg.getDefaultLanguage();
    QFont newFont=cfg.getDefaultFont();
    command_retention=cfg.getCommandRetention();
    emit commandRetention(command_retention);
    if (newFont != defaultFont) {
      defaultFont=newFont;
      emit defaultFontConfigChanged(defaultFont);
    }
  }
}

void Xpertmud::slotEditBookmark() {
  BookmarkEditor editor(bookmark,this);
  if (editor.exec() && editor.getBookmark().edited()) {
    bookmark=editor.getBookmark();
  }
}

void Xpertmud::slotNewToolbarConfig() {
  applyMainWindowSettings( KGlobal::config(), "MainWindow" );
  viewToolBar->setChecked(!toolBar("mainToolBar")->isHidden());
  viewInputBar->setChecked(!toolBar("inputBar")->isHidden());
}

void Xpertmud::slotTextEntered(const QString &text) {
  if (scriptInterp)
    scriptInterp->textEntered(text);
  else {
    send(text+"\n");
    TextBufferViewIface * statusWin= getChildTextWidget(0);
    if (statusWin) {
      //      statusWin->buffer()->print(kapp->font().family());
      statusWin->buffer()->newline();
      statusWin->buffer()->setFGColor(6);
      statusWin->buffer()->print(text);
      statusWin->buffer()->newline();
      statusWin->buffer()->resetAttributes();
    }
  }
}



void Xpertmud::slotTimer() {
  // TODO: Count every connection...
  if (scriptInterp)
    scriptInterp->timer();

  if (bytesWritten>(1<<21)) 
    statusBar()->changeItem(i18n("%1 MB").arg(bytesWritten>>20),3);
  else if (bytesWritten>10*(1<<10)) 
    statusBar()->changeItem(i18n("%1 KB").arg(bytesWritten>>10),3);
  else
    statusBar()->changeItem(i18n("%1 Bytes").arg(bytesWritten),3);

  if (bytesReceived>(1<<21))
    statusBar()->changeItem(i18n("%1 MB").arg(bytesReceived>>20),5);
  else if (bytesReceived>10*(1<<10)) 
    statusBar()->changeItem(i18n("%1 KB").arg(bytesReceived>>10),5);
  else
    statusBar()->changeItem(i18n("%1 Bytes").arg(bytesReceived),5);
  
}

void Xpertmud::saveOptions() {
  saveMainWindowSettings( config, "MainWindow" );
  config->setGroup("Colors");
  for (int i=0; i<16; ++i) {
    config->writeEntry(QString("Color%1").arg(i),
		       colorMap[i]);
  }

  config->setGroup("TextWindows");
  config->writeEntry("default-font",defaultFont);

  config->setGroup("TextBufferWindows");
  config->writeEntry("history-size",scrollbacksize);
  fileOpenRecent->saveEntries(config,"Recent Files");

  config->setGroup("Scripting");
  config->writeEntry("defaultLanguage",defaultLanguage);

  config->setGroup("InputLine");
  config->writeEntry("command-retention",command_retention);
}



void Xpertmud::readOptions() {
  applyMainWindowSettings( config, "MainWindow" );

  viewToolBar->setChecked(!toolBar("mainToolBar")->isHidden());
  viewInputBar->setChecked(!toolBar("inputBar")->isHidden());
  viewStatusBar->setChecked(!statusBar()->isHidden());
  viewQuickBar->setChecked(!toolBar("QextMdiTaskBar")->isHidden());
	
  config->setGroup("Colors");
  for (int i=0; i<16; ++i) {
    colorMap[i]=config->readColorEntry(QString("Color%1").arg(i),
				       &colorMap[i]);
  }

  {
#ifdef WIN32
    QFont myFont("fixedsys");
    myFont.setFixedPitch(true);
    myFont.setPointSize(9);
#else
    QFont myFont("Courier");
    myFont.setFixedPitch(true);
    myFont.setStyleHint(QFont::Courier);
#endif
    config->setGroup("TextWindows");
    defaultFont = config->readFontEntry("default-font",&myFont);
  }

  config->setGroup("TextBufferWindows");
  scrollbacksize = config->readNumEntry("history-size",0);

  config->setGroup("Scripting");
  defaultLanguage = config->readEntry("defaultLanguage","__DEFAULT__");

  fileOpenRecent->loadEntries(config,"Recent Files");

  config->setGroup("InputLine");
  command_retention=config->readBoolEntry("command-retention",true);
  emit commandRetention(command_retention);
}


void Xpertmud::saveProperties(KConfig *) {
  // TODO
}


void Xpertmud::readProperties(KConfig*) {
  // TODO
}


void Xpertmud::slotFileConnect() {
  if(!checkReallyDisconnect(i18n("You're about to open a new connection"))) {
    return;
  }
  slotDisconnect();

  if(bookmark.getStartInterpreter()) {
    QString preter = bookmark.getInterpreter();
    if(preter != currentLanguage || bookmark.getResetInterpreter()) {
      slotSwitchLanguage(preter);
    }
  }

  QVector<int> conns = bookmark.getAvailableConnections();
  while(conns.size() == 0) {
    BookmarkEditor editor(bookmark,this);
    if(!editor.exec())
      return;
    if(editor.getBookmark().edited()) {
      bookmark = editor.getBookmark();
      conns = bookmark.getAvailableConnections();
    }
    if(conns.size() == 0) {
      KMessageBox::error
	(this, i18n("You have to specify at least one connection."), 
	 i18n("Error!"));
    }
  }    
  for(QVector<int>::iterator it = conns.begin();
      it != conns.end(); ++it) {
    QString host=bookmark.getConnectionHost(*it);
    QString encoding=bookmark.getConnectionEncoding(*it);
    short int port=bookmark.getConnectionPort(*it);
    QString onConnect=bookmark.getConnectionScript(*it);
    
    slotConnect(*it,host,port,onConnect,encoding);

  }
  
  slotTimer(); // initialize the scripting engine...
 
/* 
  QString globalScript = bookmark.getGlobalScript();
  
  if (!globalScript.isEmpty()) {
    slotStatusMsg(i18n("Executing startup script..."));
    while (!globalScript.isEmpty()) {
      int enter=globalScript.find('\n');
      if (enter==-1) 
	enter=globalScript.length();
      QString cmd=globalScript.left(enter);
      globalScript.remove(0,enter+1);
      slotTextEntered(cmd);
    }
  }
*/
pending_commands = bookmark.getGlobalScript();
}

void Xpertmud::slotFileQuit() {
  //  cout << "quitting..." << endl;
  slotStatusMsg(i18n("Exiting..."));
  saveOptions();

  connections.reset();
  // There is only one KMainWindow...
  // and lastWindowClosed is connected to kapp::quit...
  close();

  //  slotStatusMsg(i18n("Ready."));
}

int Xpertmud::newConnection(const QString& host, int port,
		      const QString& onConnect) {
  slotStatusMsg(i18n("Connecting to %1:%2").arg(host).arg(port));

  return connections.open(host, port, onConnect,
			  this, 
			  SLOT(slotConnected(int)),
			  SLOT(slotConnectionClosed(int)),
			  SLOT(slotReceived(const QString&, int)),
			  SLOT(slotEchoChanged(bool, int)),
			  SLOT(slotBytesReceived(int, int)),
			  SLOT(slotBytesWritten(int, int)));
}

void Xpertmud::slotConnect(int id, const QString& host, int port,
			   const QString& onConnect,
			   const QString& encoding) {
  slotStatusMsg(i18n("Connecting to %1:%2").arg(host).arg(port));

  connections.open(id, host, port, onConnect,
		   this, 
		   SLOT(slotConnected(int)),
		   SLOT(slotConnectionClosed(int)),
		   SLOT(slotReceived(const QString&, int)),
		   SLOT(slotEchoChanged(bool, int)),
		   SLOT(slotBytesReceived(int, int)),
		   SLOT(slotBytesWritten(int, int)),
		   encoding);
}


void Xpertmud::slotLoadBookmark(const KURL& url) {
  if(!checkReallyDisconnect(i18n("You're about to load a new bookmark"))) {
    return;
  }

  if (bookmark.load(url)) {
    slotDisconnect();
    fileOpenRecent->addURL(url);
  }
}

void Xpertmud::slotSaveBookmark(const KURL& url) {
  if (!url.isLocalFile()) {
    KMessageBox::error(this,i18n("Sorry, only saving to local files implemented"));
    return;
  }
  if (!bookmark.save(url)) {
    KMessageBox::error(this,i18n("Saving to %1 failed").arg(url.fileName()));
  } else {
    fileOpenRecent->addURL(url);
  }
}

void Xpertmud::slotDisconnect() {
  slotStatusMsg(i18n("Disonnecting."));

  
  //  socket->close();
  connections.close(0);

  // Das  ist hier eigentlich falsch.... 
  // sollte der slotSocketDisconnected Callback machen
  fileConnect->setEnabled(true);
  fileDisconnect->setEnabled(false);
  slotStatusMsg(i18n("Ready."));
}



void Xpertmud::slotClose(int id) {
  connections.close(id);
}

void Xpertmud::slotSetInputEncoding(const QString& enc, int id) {
  connections.setInputEncoding(enc, id);
}

void Xpertmud::slotSetOutputEncoding(const QString& enc, int id) {
  connections.setOutputEncoding(enc, id);
}

void Xpertmud::setEchoMode(bool state) {
  // Hrmm?!
  slotEchoChanged(state,0);
}


void Xpertmud::slotEchoChanged(bool to, int id) {
  if(scriptInterp)
    scriptInterp->echo(to, id);
}


void Xpertmud::slotReceived(const QString& text, int id) {
  if (scriptInterp) {
    scriptInterp->textReceived(text, id);
  } else {
    printToStatusWin(text);
  }
}

void Xpertmud::slotConnected(int id) {
  // TODO: code correct
  fileConnect->setEnabled(false);
  fileDisconnect->setEnabled(true);

  if(scriptInterp)
    scriptInterp->connectionEstablished(id);
  /*
hmmm, a new implementation would only be able to
send, not to act like textEntered... 
 */
  if (!pending_commands.isEmpty()) {
    slotStatusMsg(i18n("Executing startup script..."));
    while (!pending_commands.isEmpty()) {
      int enter=pending_commands.indexOf('\n');
      if (enter==-1) 
	enter=pending_commands.length();
      QString cmd=pending_commands.left(enter);
      pending_commands.remove(0,enter+1);
      slotTextEntered(cmd);
    }
  }
  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotConnectionClosed(int id) {
  // TODO: code correct
  fileConnect->setEnabled(true);
  fileDisconnect->setEnabled(false);

  if(scriptInterp)
    scriptInterp->connectionDropped(id);

  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::slotBytesWritten(int nr, int) {
  bytesWritten += nr;
}

void Xpertmud::slotBytesReceived(int nr, int) {
  bytesReceived += nr;
}

// DCOP funcs
void Xpertmud::doConnect(const QString & host,/* short unsigned */int port) {
  slotConnect(0,host,port,"","");
}

void Xpertmud::doDisconnect() {
  slotDisconnect();
}


void Xpertmud::send(const QString & text) {
  connections.send(text, 0);
}


void Xpertmud::resizeEvent(QResizeEvent *r) {
  // QObject eventFilter is public, menuBar()->eventFilter
  // is private, so we're cheating here ;-)
  ((QObject*)menuBar())->eventFilter(this, r); 

  KMainWindow::resizeEvent(r);
}

// TODO: Use kde debug mechanism
#include <iostream>
using std::cout;
using std::endl;

bool Xpertmud::eventFilter( QObject *o, QEvent *x )
{
  if ( x->type() == QEvent::KeyPress ) {  // key press
    //std::cout << "(QT) ActiveWindow: " << (int)activeWindow() << " - ";
    //std::cout << activeWindow()->className() << std::endl;
    //std::cout << "(QT) FocusWidget: " << (int)qApp->focusWidget() << " - ";
    //std::cout << qApp->focusWidget()->className() << std::endl;

    //    cout << "Got an QEvent" << endl;
    kapp->removeEventFilter(this);
    kapp->installEventFilter(this);

    QKeyEvent *e = (QKeyEvent*)x;
    Qt::KeyboardModifiers mod=e->modifiers();
    
    QString keyname;

    keyname += (mod & Qt::ShiftModifier)?'1':'0';
    keyname += (mod & Qt::ControlModifier)?'1':'0';
    keyname += (mod & Qt::AltModifier)?'1':'0';
    //    keyname += (mod & Qt::MetaButton)?'1':'0'; 
    // not in Qt2.3 (!)
    keyname += '0';
    keyname += '0'; // Alt Gr
    keyname += '0'; // Num Lock
    keyname += '0'; // Caps Lock
    keyname += '0'; // Scroll Lock
    keyname+=' ';

    if(mod & Qt::KeypadModifier) {
      keyname+="KP_";
    }

    //QString qname = QAccel::keyToString(e->key());
    QKeySequence ks(e->key());
    QString qname = ks.toString();
    if(qname == "<65535?>") {
      keyname += "KP_Begin";
    } else if(qname == "<4128?>") {
      keyname += "Shift_L";
    } else if(qname == "<4129?>") {
      keyname += "Control_L";
    } else if(qname == "<4131?>") {
      keyname += "Alt_L";
    } else if(qname == "<0?>") {
      keyname += "NoSymbol";
    } else {
      keyname += qname;
    }
    if (scriptInterp)
      return scriptInterp->keyPressed(keyname, e->text());
  }
  return KMainWindow::eventFilter( o, x );    // standard event processing
}




