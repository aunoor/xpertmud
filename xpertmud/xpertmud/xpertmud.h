// -*- c++ -*-
/***************************************************************************
   $Header$
                             -------------------
    begin                : Tue Jun 19 11:36:10 MEST 2001
    copyright            : (C) 2001,2002 by Ernst Bachmann, Manuel Klimek

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XPERTMUD_H
#define XPERTMUD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

# define USE_QT_KEYHANDLING

#include <kapplication.h>
#include <kmainwindow.h>
#include <kaction.h>
#include <kurl.h>
#include <qextmdimainfrm.h>
#include <vector>
#include "scripting/GuiScriptingBindings.h"
#include "ColorChar.h"
#include "RegisterIface.h"

#include "ConnectionHandler.h"
#include "Bookmark.h"


class XMScripting;
class QSocket;
class PluginWrapper;
class KLibrary;
class MXInputLine;
class TextBufferView;
class TextBufferViewIface;
class TextBufferHistoryView;

/** Xpertmud is the base class of the project */
class Xpertmud : public QextMdiMainFrm, public GuiScriptingBindings, 
  public RegisterIface
{
  Q_OBJECT 

  typedef std::vector<Connection*> connectionListT;
public:
  /** construtor */
  Xpertmud(QWidget* parent=0, const char *name="");
  /** destructor */
  ~Xpertmud();
  
  // Initialize stuff that cannot be handled in the constructor
  void initialize();

public: // Interface RegisterIface
  unsigned int registerWidget(QWidget* widget);
  void unregisterWidget(QWidget* widget);
  void unregisterWidget(unsigned int id);
  QWidget* getRegisteredWidget(unsigned int id);
  

public: // Interface GuiScriptingBindings
  void XM_Window_setTitle(int id, const QString& string);
  void XM_Window_show(int id);
  void XM_Window_hide(int id);
  void XM_Window_close(int id);
  void XM_Window_minimize(int id);
  void XM_Window_maximize(int id);
  void XM_Window_move(int id,int x, int y);
  void XM_Window_resize(int id,int x, int y);
  bool XM_Window_isValid(int id);
  void XM_Window_raise(int id);
  void XM_Window_lower(int id);
  int  XM_Window_getPositionX(int id);
  int  XM_Window_getPositionY(int id);
  int  XM_Window_getSizeX(int id);
  int  XM_Window_getSizeY(int id);
  void XM_Window_showCaption(int id);
  void XM_Window_hideCaption(int id);


  int  XM_TextWindow_initialize();
  void XM_TextWindow_newline(int id);
  void XM_TextWindow_clear(int id);
  void XM_TextWindow_clearLine(int id);
  void XM_TextWindow_clearEOL(int id);
  void XM_TextWindow_clearBOL(int id);
  void XM_TextWindow_clearEOS(int id);
  void XM_TextWindow_clearBOS(int id);
  void XM_TextWindow_resetAttributes(int id);
  void XM_TextWindow_printRaw(int id,const QString& string);
  void XM_TextWindow_print(int id,const QString& string);
  void XM_TextWindow_setBGColor(int id,int color);
  void XM_TextWindow_setFGColor(int id,int color);
  void XM_TextWindow_setDefaultBGColor(int id,int color);
  void XM_TextWindow_setDefaultFGColor(int id,int color);
  void XM_TextWindow_scrollLines(int id,int lines);
  void XM_TextWindow_scrollColumns(int id,int cols);
  void XM_TextWindow_setBold(int id,bool bold);
  void XM_TextWindow_setBlinking(int id,bool bold);
  void XM_TextWindow_setUnderline(int id,bool bold);
  void XM_TextWindow_setReverse(int id,bool bold);
  void XM_TextWindow_setIntensive(int id,bool bold);
  void XM_TextWindow_setCursor(int id,unsigned int x,unsigned int y);
  void XM_TextWindow_resizeChars(int id,int x, int y);
  int  XM_TextWindow_getCursorX(int id);
  int  XM_TextWindow_getCursorY(int id);
  int  XM_TextWindow_getColumns(int id);
  int  XM_TextWindow_getLines(int id);
  char XM_TextWindow_getCharAt(int id,unsigned int x, unsigned int y); 
  int  XM_TextWindow_getFGColorAt(int id,unsigned int x, unsigned int y);
  int  XM_TextWindow_getBGColorAt(int id,unsigned int x, unsigned int y);
  bool XM_TextWindow_setFont(int id,const QString& string, int size);
  void XM_TextWindow_enableMouseEvents(int id);
  void XM_TextWindow_disableMouseEvents(int id);

  int  XM_TextBufferWindow_initialize();
  int  XM_TextBufferWindow_getWordWrapColumn(int id);
  void XM_TextBufferWindow_setWordWrapColumn(int id, int col);
  //  void XM_TextBufferWindow_scrollView(int id, int dx,int dy);

  int  XM_Plugin_initialize(const QString& libname,
			    const QString& classname);
  QString XM_Plugin_call(int id, int fnum, const QString& param);

  int XM_Connection_initialize(const QString& host, int port, const QString& onConnect="");
  void XM_Connection_open(int id, const QString& host, int port, const QString& onConnect="");
  void XM_Connection_send(int id, const QString& string);
  void XM_Connection_close(int id);
  void XM_Connection_setInputEncoding(int id, const QString& enc);
  void XM_Connection_setOutputEncoding(int id, const QString& enc);

  // funcs for InputLines
  int XM_InputLine_getCursorX(int id);
  int XM_InputLine_getCursorY(int id);
  void XM_InputLine_setCursorX(int id, int x);
  void XM_InputLine_setCursorY(int id, int y);
  QString XM_InputLine_getText(int id);
  void XM_InputLine_setText(int id, const QString& text);

  void XM_echoMode(bool);
  int XM_getActiveWindow();
  void XM_send(const QString& string);
  int XM_getFocusedInputLine();
  void XM_setStatusMsg(const QString& text);
  void XM_resetStatusMsg();
  void XM_showError(const QString& string);
  int  XM_mdiWidth();
  int  XM_mdiHeight();
  void XM_playAudio(const QString& filename);
  QString XM_ansiToRaw(const QString& string);
  QString XM_ansiToColored(const QString& string);

public: // implementation of DCOP Iface
  void doConnect(const QString & host,/* short unsigned */int port);
  void doDisconnect();

  // send to active connection
  void send(const QString & text);

protected:
  // Zugiffsfunktionen auf Child-Widgets
  QWidget * getChildQWidget(unsigned int id);
  QextMdiChildView * getChildWrapper(unsigned int id);
  TextBufferViewIface * getChildTextWidget(unsigned int id);
  TextBufferHistoryView * getChildTextBufferWidget(unsigned int id);
  QWidget* getChildWidget(unsigned int id);
  PluginWrapper * getPluginWrapper(unsigned int id);
  void printToStatusWin(const QString & text);

  MXInputLine* getInputLine(unsigned int id);

protected:
  virtual void resizeEvent(QResizeEvent *r);

  bool eventFilter( QObject *, QEvent * );

  /** saves the window properties for each open window during session end to the session config file, including saving the currently
   * opened file by a temporary filename provided by KApplication.
   * @see KTMainWindow#saveProperties
   */
  virtual void saveProperties(KConfig *_cfg);
  /** reads the session config file and restores the application's state including the last opened files and documents by reading the
   * temporary files saved by saveProperties()
   * @see KTMainWindow#readProperties
   */
  virtual void readProperties(KConfig *_cfg);

  virtual bool queryExit();
  virtual bool queryClose();

private:
  int newConnection(const QString&, int, const QString&);
  bool checkReallyDisconnect(const QString& what);
  void initStatusBar();
  void initActions();
  void initView();
  /** save general Options like all bar positions and status as well as the geometry and the recent file list to the configuration
   * file
   */ 	
  void saveOptions();
  /** read general Options again and initialize all variables like the recent file list
   */
  void readOptions();

  bool saveIfModified();

private slots:
  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotEditBookmark();

  void slotFindLanguages();
   
  void slotSwitchLanguage(const QString &lang);
  void slotSwitchLanguage(int);
  void slotUnloadLanguage();
  void windowMenuAboutToShow();
  void scriptingMenuAboutToShow();
  void slotStatusMsg(const QString &text);
  
  /** toggles the toolbar
   */
  void slotViewToolBar();

  /** toggles the toolbar
   */
  void slotViewInputBar();
  
  /** toggles the statusbar
   */
  void slotViewStatusBar();

  void slotViewQuickBar();
  
  /** closes all documents and quits the application.*/
  void slotFileQuit();
  
  // Called from InputAction/mxinputline
  void slotTextEntered(const QString &);

  void slotTimer();
  /** activates the MDI child widget when it gets selected from the window menu. */
  //  void windowMenuActivated( int id );

  /**  */
  void slotWindowTile();
  /**  */
  void slotWindowCascade();

  void slotFileConnect();

  void slotEditToolbars();
  void slotNewToolbarConfig();
  void slotConfigure();
  
  // ScriptingMouseEvent stuff
private slots:
  void slotScriptingMousePress(int id, int x, int y);
  void slotScriptingMouseDrag(int id, int x, int y);
  void slotScriptingMouseRelease(int id, int x, int y);

  // PluginEvent stuff
private slots:
  void pluginCallbackCollector(int id, int func,const QVariant& args, QVariant & result);

  // Bookmark handling stuff
public slots:
  void slotLoadBookmark(const KURL&);
  void slotSaveBookmark(const KURL&);

private slots:
  void slotConnect(int, const QString&, int, const QString&, const QString&);
  void slotDisconnect();
  void slotClose(int id);
  void slotSetInputEncoding(const QString& enc, int id);
  void slotSetOutputEncoding(const QString& enc, int id);
  void slotEchoChanged(bool, int);

  void setEchoMode(bool); // for inputToggleAction, calls slotEchoChanged for connection 0

// TODO: delete  void slotSocketReady();
  void slotConnected(int); // id
  void slotConnectionClosed(int); // id
  void slotReceived(const QString&, int); // text, id
  void slotBytesWritten(int, int); // nr, id
  void slotBytesReceived(int, int); // nr, id
// TODO: delete  void slotConnectionError(int);

signals:
  void colorConfigChanged(); 
  void historyConfigChanged(int lines);
  void echoMode(bool);
  void commandRetention(bool);
  void defaultFontConfigChanged(const QFont & font);

private:
  KLibrary *currentLib; // to get rid of it

  /** the configuration object of the application */
  KConfig *config;

  // GUI Objects

  //  QextMdiMainFrm *pWorkspace;
  QVector<QWidget *> childWidgets;
  std::vector<QextMdiChildView *> wrapperWidgets;

  std::vector<PluginWrapper *> pluginWrappers;

  std::vector<QWidget*> registeredWidgets;

  // Actions
  KAction* fileNew;
  KAction* fileOpen;
  KAction* fileSave;
  KAction* fileSaveAs;
  KAction* fileEdit;
  KAction* fileConnect;
  KAction* fileDisconnect;
  KAction* fileQuit;
  KRecentFilesAction* fileOpenRecent;

  KAction* windowTile;
  KAction* windowCascade;
  KAction* configureToolbars;
  KAction* preferences;
  KAction* inputAction;
  KAction* unloadLanguage;

  KToggleAction* viewToolBar;
  KToggleAction* viewInputBar;
  KToggleAction* viewStatusBar;
  KToggleAction* viewQuickBar;
  KToggleAction* echoModeAction;
  KActionMenu* windowMenu;
  KActionMenu* scriptingMenu;

  // Non-GUI Objects
  XMScripting *scriptInterp;
  QStringList availableLanguages;
  QString currentLanguage;
  QString defaultLanguage;
  QString scriptLibName;

  ConnectionHandler connections;
  Bookmark bookmark;
  QString pending_commands;

  // Configuration
  QColor * colorMap;
  int scrollbacksize;
  QFont defaultFont;

  int bytesWritten;
  int bytesReceived;
  bool command_retention;
};

#endif
