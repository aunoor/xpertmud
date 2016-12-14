/***************************************************************************
   $Header$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klibloader.h>
#include <klocale.h>
#include <kaudioplayer.h>

#include "xpertmud.h"
#include "PluginWrapper.h"
#include "TextBuffer.h"
#include "TextBufferView.h"
#include "TextBufferHistoryView.h"
#include "scripting/Scripting.h"
#include "CtrlParser.h"
#include "mxinputline.h"

// TEMPORARY DEBUG
#include <iostream>


////////////////////////////////////////////////////////////////////////////
// Scripting Callbacks

void Xpertmud::XM_Window_setTitle(int id,const QString& string) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    qw->setCaption(string);
    qw->setTabCaption(string);
    qw->setMDICaption(string);
  }
}

void Xpertmud::XM_Window_show(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    //    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->show();
  }
}

void Xpertmud::XM_Window_hide(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    //    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->hide();
  }
}

void Xpertmud::XM_Window_close(int id) {
  //  TextBufferViewIface * tw=getChildTextWidget(id);
  QextMdiChildView * qw=getChildWrapper(id);
  if (id && qw) { // statusWindow cannot be deleted
    //    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    //    qw->hide();
    //    detachWindow(qw);
    removeWindowFromMdi(qw);
    childWidgets[id]=NULL;
    wrapperWidgets[id]=NULL;
    pluginWrappers[id]=NULL;
    delete qw;
  }
}

void Xpertmud::XM_Window_minimize(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    //    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->minimize(); 
  }
}

void Xpertmud::XM_Window_maximize(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    //if(qw->parentWidget()) { qw = qw->parentWidget(); }
    if(!qw->parentWidget()) {
      qw->hide();
      qw->maximize();
    } else {
      qw->show();
      qw->maximize();
      //      qw->mdiParent()->setState(QextMdiChildFrm::Maximized);
    }
    qw->activate();
  }
}

void Xpertmud::XM_Window_move(int id, int x, int y) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->move(x,y);
  }
}

void Xpertmud::XM_Window_resize(int id, int x, int y) {
  QextMdiChildView * qw=getChildWrapper(id);

  if (qw) {
    if(!qw->parentWidget()) {
      qw->showNormal();
      qw->resize(x, y);
    } else {
      qw->mdiParent()->restorePressed();
      qw->mdiParent()->resize(x, y);
    }
  }
}

bool Xpertmud::XM_Window_isValid(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw)
    return true;
  else
    return false;
}

void Xpertmud::XM_Window_raise(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->raise();
  }
}
void Xpertmud::XM_Window_lower(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    qw->lower();
  }
}

int Xpertmud::XM_Window_getPositionX(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    return qw->x();
  } else
    return -1;
}
int Xpertmud::XM_Window_getPositionY(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    return qw->y();
  } else
    return -1;
}
int Xpertmud::XM_Window_getSizeX(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    return qw->width();
  } else
    return -1;
}
int Xpertmud::XM_Window_getSizeY(int id) {
  QWidget * qw=getChildWrapper(id);
  if (qw) {
    if(qw->parentWidget()) { qw = qw->parentWidget(); }
    return qw->height();
  } else
    return -1;
}

void Xpertmud::XM_Window_showCaption(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw && qw->mdiParent() != NULL)
    qw->mdiParent()->showCaption();
}

void Xpertmud::XM_Window_hideCaption(int id) {
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw && qw->mdiParent() != NULL)
    qw->mdiParent()->hideCaption();
}


int Xpertmud::XM_TextWindow_initialize() {
  int id=childWidgets.size(); 

  QextMdiChildView * neuWrapper = new QextMdiChildView();
  QBoxLayout* pLayout = new QHBoxLayout(neuWrapper, 0, -1);
  TextBufferView * neu= new TextBufferView(id,neuWrapper,0,colorMap,defaultFont);
  pLayout->addWidget(neu);

  childWidgets.push_back(neu);  
  wrapperWidgets.push_back(neuWrapper);
  pluginWrappers.push_back(NULL); 

  neuWrapper->setIcon(kapp->miniIcon());
  neuWrapper->setFocusPolicy(QWidget::NoFocus);
  neuWrapper->removeEventFilterForAllChildren();

  connect(this,SIGNAL(colorConfigChanged()),
	  neu,SLOT(slotColorConfigChanged()));
  connect(this,SIGNAL(defaultFontConfigChanged(const QFont &)),
	  neu,SLOT(slotNewDefaultFont(const QFont &)));

  addWindow(neuWrapper, QextMdi::Hide);

  return id;
}

void Xpertmud::XM_TextWindow_newline(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->newline();
}
void Xpertmud::XM_TextWindow_clear(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearWindow();
}
void Xpertmud::XM_TextWindow_clearLine(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearLine();
}
void Xpertmud::XM_TextWindow_clearEOL(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearEOL();
}
void Xpertmud::XM_TextWindow_clearBOL(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearBOL();
}
void Xpertmud::XM_TextWindow_clearEOS(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearEOS();
}
void Xpertmud::XM_TextWindow_clearBOS(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->clearBOS();
}
void Xpertmud::XM_TextWindow_resetAttributes(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->resetAttributes();
}
void Xpertmud::XM_TextWindow_printRaw(int id,const QString& string) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->printRaw(string);
}
void Xpertmud::XM_TextWindow_print(int id,const QString& string) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->print(string);
}
void Xpertmud::XM_TextWindow_setBGColor(int id,int color) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setBGColor(color);
}
void Xpertmud::XM_TextWindow_setFGColor(int id,int color) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setFGColor(color);
}
void Xpertmud::XM_TextWindow_setDefaultBGColor(int id,int color) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setDefaultBGColor(color);
}
void Xpertmud::XM_TextWindow_setDefaultFGColor(int id,int color) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setDefaultFGColor(color);
}
void Xpertmud::XM_TextWindow_scrollLines(int id,int lines) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->scrollLines(lines);
}
void Xpertmud::XM_TextWindow_scrollColumns(int id,int cols) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->scrollColumns(cols);
}
void Xpertmud::XM_TextWindow_setBold(int id,bool bold) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setBold(bold);
}
void Xpertmud::XM_TextWindow_setBlinking(int id,bool blinking) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setBlinking(blinking);
}
void Xpertmud::XM_TextWindow_setUnderline(int id,bool underline) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setUnderline(underline);
}
void Xpertmud::XM_TextWindow_setReverse(int id,bool reverse) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setReverse(reverse);
}
void Xpertmud::XM_TextWindow_setIntensive(int id,bool intense) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setIntense(intense);
}

void Xpertmud::XM_TextWindow_setCursor(int id, unsigned int x, unsigned int y) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    tw->buffer()->setCursor(x,y);
}

void Xpertmud::XM_TextWindow_resizeChars(int id, int x, int y) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  QextMdiChildView * qw=getChildWrapper(id);
  if (qw) {
    QSize s = tw->sizeForChars(x, y);

    if(!qw->parentWidget()) {
      qw->showNormal();
    } else {
      qw->mdiParent()->restorePressed();
    }
    qw->setInternalGeometry(QRect(QPoint(qw->x(),qw->y()),s));
  }
}

int Xpertmud::XM_TextWindow_getCursorX(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getCursorX();
  else
    return -1;
}
int Xpertmud::XM_TextWindow_getCursorY(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getCursorY();
  else
    return -1;
}
int Xpertmud::XM_TextWindow_getColumns(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getColumns();
  else
    return -1;
}
int Xpertmud::XM_TextWindow_getLines(int id) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getLines();
  else
    return -1;
}
char Xpertmud::XM_TextWindow_getCharAt(int id, unsigned int x, unsigned int y) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getCharAt(x,y);
  else
    return 0;
}
int Xpertmud::XM_TextWindow_getFGColorAt(int id, unsigned int x, unsigned int y) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getColorCharAt(x,y).getFg();
  else
    return 0;
}
int Xpertmud::XM_TextWindow_getBGColorAt(int id, unsigned int x, unsigned int y) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw)
    return tw->buffer()->getColorCharAt(x,y).getBg();
  else
    return 0;
}
bool Xpertmud::XM_TextWindow_setFont(int id,const QString& text,int size) {
  TextBufferViewIface * tw=getChildTextWidget(id);
  if (tw) {
    QFont newfont(text,size);
    if(QString(text) == "default" || QString(text) == "fixed") {
      newfont = defaultFont;
      newfont.setPointSize(size);
    }
    newfont.setFixedPitch(true);
    //newfont.setCharSet(QFont::Latin1);
    tw->setScriptingFont(newfont);
    return true;
  } else
    return false;
}
void Xpertmud::XM_TextWindow_enableMouseEvents(int id) {
  QWidget* tw = getChildWidget(id);
  TextBufferViewIface* ti = getChildTextWidget(id);
  if (tw && ti) {
    ti->switchMouseMode(false);
    connect(tw,SIGNAL(scriptingMousePressEvent(int, int, int)),
	    this,SLOT(slotScriptingMousePress(int, int, int )));
    connect(tw,SIGNAL(scriptingMouseDragEvent(int, int, int)),
	    this,SLOT(slotScriptingMouseDrag(int, int, int)));
    connect(tw,SIGNAL(scriptingMouseReleaseEvent(int, int, int)),
	    this,SLOT(slotScriptingMouseRelease(int, int, int)));
  }
}
void Xpertmud::XM_TextWindow_disableMouseEvents(int id) {
  QWidget* tw = getChildWidget(id);
  TextBufferViewIface* ti = getChildTextWidget(id);
  if (tw && ti) {
    ti->switchMouseMode(true);
    disconnect(tw,SIGNAL(scriptingMousePressEvent(int, int, int)),
  	       this,SLOT(slotScriptingMousePress(int, int, int)));
    disconnect(tw,SIGNAL(scriptingMouseDragEvent(int, int, int)),	       
	       this,SLOT(slotScriptingMouseDrag(int, int, int)));
    disconnect(tw,SIGNAL(scriptingMouseReleaseEvent(int, int, int)),
	       this,SLOT(slotScriptingMouseRelease(int, int, int)));
  }
}

int Xpertmud::XM_TextBufferWindow_initialize() {
  int id=childWidgets.size(); 
  QextMdiChildView * neuWrapper = new QextMdiChildView();
  TextBufferHistoryView * neu= new TextBufferHistoryView(id, neuWrapper,0,colorMap,
					       defaultFont);
  QBoxLayout* pLayout = new QHBoxLayout(neuWrapper, 0, -1);
  pLayout->addWidget(neu);
  neuWrapper->hide();

  childWidgets.push_back(neu);  
  wrapperWidgets.push_back(neuWrapper);
  pluginWrappers.push_back(NULL);

  neuWrapper->setIcon(kapp->miniIcon());
  neuWrapper->setFocusPolicy(QWidget::NoFocus);
  neuWrapper->removeEventFilterForAllChildren();
  neu->buffer()->setMaxBufferSize(scrollbacksize);

  // TODO:
#pragma warning TODO
  connect(this,SIGNAL(colorConfigChanged()),
	  neu,SLOT(slotColorConfigChanged()));
  
#pragma warning TODO
  connect(this,SIGNAL(historyConfigChanged(int)),
	  neu->buffer(),SLOT(setMaxBufferSize(int)));

#pragma warning TODO
  connect(this,SIGNAL(defaultFontConfigChanged(const QFont &)),
	  neu,SLOT(slotNewDefaultFont(const QFont &)));

  addWindow(neuWrapper, QextMdi::Hide);

  return id;
}
int Xpertmud::XM_TextBufferWindow_getWordWrapColumn(int id) {
  TextBufferViewIface * tbw = getChildTextBufferWidget(id);
  if (tbw) {
    return tbw->buffer()->getWordWrapColumn();
  }
  return 0;
}
void Xpertmud::XM_TextBufferWindow_setWordWrapColumn(int id, int col) {
  TextBufferViewIface * tbw = getChildTextBufferWidget(id);
  if (tbw) {
    tbw->buffer()->setWordWrapColumn(col);
  }
}

int Xpertmud::XM_Plugin_initialize(const QString& libname,
				   const QString& classname) {
  // Remember old focus .. this is a evil Workaround for a QT bug
  QWidget * oldFocus=focusWidget();

  QWidget * plugin=NULL;

  KLibFactory * fac = KLibLoader::self()->factory(libname);
  if (fac) {
    QextMdiChildView * neuWrapper = new QextMdiChildView();
    QObject * plg = fac->create(neuWrapper,NULL,classname);

    if (plg && plg->inherits("QWidget")) {
      plugin=static_cast<QWidget*>(plg);
    } else {
      if (plg)
	delete plg;
      if (neuWrapper)
	delete neuWrapper;
    }
    if (plugin) {
      QBoxLayout* pLayout = new QHBoxLayout(neuWrapper, 0, -1);
      pLayout->addWidget(plugin);

      int id=childWidgets.size(); 
      plugin->setFocusPolicy( QWidget::NoFocus );

      neuWrapper->removeEventFilterForAllChildren();

      plugin->show();

      wrapperWidgets.push_back(neuWrapper);
      childWidgets.push_back(plugin);
      PluginWrapper * pw=new PluginWrapper(plugin,id);
      pluginWrappers.push_back(pw);

      //ASSERT(childWidgets.size()==pluginWrappers.size());
      
      connect(pw,SIGNAL(callback(int, int, const QVariant &, QVariant &)),
	      this,SLOT(pluginCallbackCollector(int, int, const QVariant &, QVariant &)));

      addWindow(neuWrapper, QextMdi::Hide);

      // BUGFIX/Workaround
      if (oldFocus!=NULL)
	oldFocus->setFocus();
      
      return id;
    } else {
      // TODO: Segfaults
      //     KMessageBox::error(this,i18n("Library %1 loaded, but no Plugin %2 created").arg(libname).arg(classname), i18n("Error!"));
      return -1;
    }
  } else {
    // TODO: Segfaults
    //    KMessageBox::error(this,i18n("Library could not be loaded: \n%1").arg(KLibLoader::self()->lastErrorMessage()),
    //		       i18n("Error while loading %1!").arg(classname));
    return -1;
  }
}

int Xpertmud::XM_Connection_initialize(const QString& host, int port, const QString& onConnect) {
  return newConnection(host, port, onConnect);
}

void Xpertmud::XM_Connection_open(int id, const QString&  host,
				  int port, const QString& onConnect) {
  slotConnect(id, host, port, onConnect, "");
}

void Xpertmud::XM_Connection_send(int id, const QString& string) {
  connections.send(string, id);
}

void Xpertmud::XM_Connection_close(int id) {
  slotClose(id);
}

void Xpertmud::XM_Connection_setInputEncoding(int id, const QString& enc) {
  slotSetInputEncoding(enc, id);
}

void Xpertmud::XM_Connection_setOutputEncoding(int id, const QString& enc) {
  slotSetOutputEncoding(enc, id);
}

int Xpertmud::XM_getActiveWindow() {
  QWidget* active = activeWindow();
  if(active->inherits("QextMdiChildView")) {
  QextMdiChildView* wrapper = 
    static_cast<QextMdiChildView*>(active);
    for(unsigned int id=0; id<wrapperWidgets.size(); ++id) {
      if(wrapperWidgets[id] == wrapper) {
	return id;
      }
    }
  }
  return -1;
}

void Xpertmud::XM_send(const QString& string) {
  connections.send(string, 0);
}

int Xpertmud::XM_getFocusedInputLine() {
  QWidget* focus = qApp->focusWidget();

  if(focus->inherits("MXInputLine")) {
    MXInputLine* inputLine =
      static_cast<MXInputLine*>(focus);
    return inputLine->getID();
  }

  return -1;
}

void Xpertmud::XM_setStatusMsg(const QString& text) {
  slotStatusMsg(text);
}

void Xpertmud::XM_resetStatusMsg() {
  slotStatusMsg(i18n("Ready."));
}

void Xpertmud::XM_showError(const QString& string) {
  TextBufferViewIface * statusWin= getChildTextBufferWidget(0);
  if (statusWin) {
    statusWin->buffer()->setFGColor(1);
    statusWin->buffer()->setBold(true);
    statusWin->buffer()->setIntense(true);
    statusWin->buffer()->newline();
    statusWin->buffer()->print(string); 
    statusWin->buffer()->resetAttributes();
  }
   // TODO: Display this with other means ( Dialog-Box, etc
}


void Xpertmud::XM_echoMode(bool state) {
  std::cout << "got an echo change " << (int)state << std::endl;
  emit echoMode(state);
}

int Xpertmud::XM_mdiWidth() {
  return m_pMdi->width();
}

int Xpertmud::XM_mdiHeight() {
  return m_pMdi->height();
}

QextMdiChildView * Xpertmud::getChildWrapper(unsigned int id) {
  if (id>=childWidgets.size()) return NULL;
  return wrapperWidgets[id];
}

TextBufferViewIface * Xpertmud::getChildTextWidget(unsigned int id) {
  if (id>=childWidgets.size()) return NULL;
  QWidget * qw = childWidgets[id];
  if (!qw) return NULL;
  TextBufferViewIface* ret=NULL;

  if (qw->inherits("TextBufferHistoryView")) 
      ret=static_cast<TextBufferViewIface*>(static_cast<TextBufferHistoryView*>(qw));

  if (qw->inherits("TextBufferView")) 
      ret=static_cast<TextBufferViewIface*>(static_cast<TextBufferView*>(qw));


  return ret;
}

TextBufferHistoryView * Xpertmud::getChildTextBufferWidget(unsigned int id) {
  if (id>=childWidgets.size()) return NULL;
  QWidget * qw = childWidgets[id];
  if (!qw) return NULL;
  if (qw->inherits("TextBufferHistoryView")) return static_cast<TextBufferHistoryView *>(qw);
  return NULL;
}

QWidget * Xpertmud::getChildWidget(unsigned int id) {
  if (id>=childWidgets.size()) return NULL;
  return childWidgets[id];
}

PluginWrapper * Xpertmud::getPluginWrapper(unsigned int id) {
  if (id>=childWidgets.size()) return NULL;
  return pluginWrappers[id];
}


void Xpertmud::pluginCallbackCollector(int id, int func,const QVariant& args, QVariant & result) {
  //ASSERT(getPluginWrapper(id) != NULL);
  if (scriptInterp)
    result=scriptInterp->pluginCall(id,func,args);
  
}

void Xpertmud::slotScriptingMousePress(int id, int x, int y) {
  if (scriptInterp)
    scriptInterp->mouseDown(id,x,y);
}

void Xpertmud::slotScriptingMouseDrag(int id, int x, int y) {
  if (scriptInterp)
    scriptInterp->mouseDrag(id,x,y);
}

void Xpertmud::slotScriptingMouseRelease(int id, int x, int y) {
  if (scriptInterp)
    scriptInterp->mouseUp(id,x,y);
}

QString Xpertmud::XM_Plugin_call(int id, int fnum, const QString& arg) {
  PluginWrapper * pw=getPluginWrapper(id);
  if (pw) {
    QVariant result;
    pw->slotFunctionCall(fnum,arg,result);
    if (result.isValid()) {
      QString r=result.toString();
      return r;
    }
  }
  return NULL;
}

QString Xpertmud::XM_ansiToRaw(const QString& string) {
  CtrlParser parser;
  parser.parse(string);

  return parser.returnStripped();
}

QString Xpertmud::XM_ansiToColored(const QString& string) {
  CtrlParser parser;
  parser.parse(string);

  return parser.returnColored();
}

int Xpertmud::XM_InputLine_getCursorX(int id) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL) {
    int para, index;
    inputLine->getCursorPosition(&para, &index);
    return index;
  }
  return 0;
}
int Xpertmud::XM_InputLine_getCursorY(int id) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL) {
    int para, index;
    inputLine->getCursorPosition(&para, &index);
    return para;
  }
  return 0;
}

void Xpertmud::XM_InputLine_setCursorX(int id, int x) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL) {
    int para, index;
    inputLine->getCursorPosition(&para, &index);
    inputLine->setCursorPosition(para, x);
  }
}

void Xpertmud::XM_InputLine_setCursorY(int id, int y) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL) {
    int para, index;
    inputLine->getCursorPosition(&para, &index);
    inputLine->setCursorPosition(y, index);
  }
}

QString Xpertmud::XM_InputLine_getText(int id) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL)
    return inputLine->text();
  return "";
}

void Xpertmud::XM_InputLine_setText(int id, const QString& text) {
  MXInputLine* inputLine = getInputLine(id);
  if(inputLine != NULL)
    inputLine->setText(text);
}

MXInputLine* Xpertmud::getInputLine(unsigned int id) {
  QWidget* w = getRegisteredWidget(id);

  if (w && w->inherits("MXInputLine")) 
    return static_cast<MXInputLine*>(w);
  
  return NULL;
}

void Xpertmud::XM_playAudio(const QString& filename) {
   KAudioPlayer::play(filename);
}
