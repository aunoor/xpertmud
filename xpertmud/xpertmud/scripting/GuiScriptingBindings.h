// -*- c++ -*-
#ifndef GUISCRIPTINGBINDINGS_H
#define GUISCRIPTINGBINDINGS_H

#ifndef ABSTRACT
#  define ABSTRACT 0
#endif

#include <qstring.h>

/**
 * Strings are represented QString now!
 */
class GuiScriptingBindings { // pure virtual interface
public:
  // funcs for QWidget
  virtual void XM_Window_close(int id) = ABSTRACT;
  virtual int  XM_Window_getPositionX(int id) = ABSTRACT;
  virtual int  XM_Window_getPositionY(int id) = ABSTRACT;
  virtual int  XM_Window_getSizeX(int id) = ABSTRACT;
  virtual int  XM_Window_getSizeY(int id) = ABSTRACT;
  virtual void XM_Window_hide(int id) = ABSTRACT;
  virtual void XM_Window_hideCaption(int id) = ABSTRACT;
  virtual bool XM_Window_isValid(int id) = ABSTRACT;
  virtual void XM_Window_lower(int id) = ABSTRACT;
  virtual void XM_Window_maximize(int id) = ABSTRACT;
  virtual void XM_Window_minimize(int id) = ABSTRACT;
  virtual void XM_Window_move(int id,int x, int y) = ABSTRACT;
  virtual void XM_Window_raise(int id) = ABSTRACT;
  virtual void XM_Window_resize(int id,int x, int y) = ABSTRACT;
  virtual void XM_Window_setTitle(int id,const QString& string) = ABSTRACT;
  virtual void XM_Window_show(int id) = ABSTRACT;
  virtual void XM_Window_showCaption(int id) = ABSTRACT;

  // funcs for TextWidget
  virtual int XM_TextWindow_initialize() = ABSTRACT;
  virtual void XM_TextWindow_clear(int id) = ABSTRACT;
  virtual void XM_TextWindow_clearEOL(int id) = ABSTRACT;
  virtual void XM_TextWindow_clearBOL(int id) = ABSTRACT;
  virtual void XM_TextWindow_clearEOS(int id) = ABSTRACT;
  virtual void XM_TextWindow_clearBOS(int id) = ABSTRACT;
  virtual void XM_TextWindow_clearLine(int id) = ABSTRACT;
  virtual void XM_TextWindow_disableMouseEvents(int id) = ABSTRACT;
  virtual void XM_TextWindow_enableMouseEvents(int id) = ABSTRACT;
  virtual char XM_TextWindow_getCharAt(int id,unsigned int x, unsigned int y) = ABSTRACT;
  virtual int  XM_TextWindow_getColumns(int id) = ABSTRACT;
  virtual int  XM_TextWindow_getCursorX(int id) = ABSTRACT;
  virtual int  XM_TextWindow_getCursorY(int id) = ABSTRACT;
  virtual int XM_TextWindow_getFGColorAt(int id,unsigned int x, unsigned int y) = ABSTRACT;
  virtual int XM_TextWindow_getBGColorAt(int id,unsigned int x, unsigned int y) = ABSTRACT;
  virtual int  XM_TextWindow_getLines(int id) = ABSTRACT;
  virtual void XM_TextWindow_newline(int id) = ABSTRACT;
  virtual void XM_TextWindow_printRaw(int id,const QString& string) = ABSTRACT;
  virtual void XM_TextWindow_print(int id,const QString& string) = ABSTRACT;
  virtual void XM_TextWindow_resetAttributes(int id) = ABSTRACT;
  virtual void XM_TextWindow_resizeChars(int id,int x, int y) = ABSTRACT;
  virtual void XM_TextWindow_scrollColumns(int id,int cols) = ABSTRACT;
  virtual void XM_TextWindow_scrollLines(int id,int lines) = ABSTRACT;
  virtual void XM_TextWindow_setBold(int id,bool bold) = ABSTRACT;
  virtual void XM_TextWindow_setBlinking(int id,bool bold) = ABSTRACT;
  virtual void XM_TextWindow_setCursor(int id,unsigned int x, unsigned int y) = ABSTRACT;
  virtual void XM_TextWindow_setDefaultBGColor(int id,int color) = ABSTRACT;
  virtual void XM_TextWindow_setDefaultFGColor(int id,int color) = ABSTRACT;
  virtual bool XM_TextWindow_setFont(int id,const QString& string, int size) = ABSTRACT;
  virtual void XM_TextWindow_setIntensive(int id,bool bold) = ABSTRACT;
  virtual void XM_TextWindow_setReverse(int id,bool bold) = ABSTRACT;
  virtual void XM_TextWindow_setUnderline(int id,bool bold) = ABSTRACT;
  virtual void XM_TextWindow_setBGColor(int id,int color) = ABSTRACT;
  virtual void XM_TextWindow_setFGColor(int id,int color) = ABSTRACT;

  // funcs for TextBufferWidget
  virtual int  XM_TextBufferWindow_initialize() = ABSTRACT;
  virtual int  XM_TextBufferWindow_getWordWrapColumn(int id) = ABSTRACT;
  virtual void XM_TextBufferWindow_setWordWrapColumn(int id, int col) = ABSTRACT;

  // funcs for Plugins
  virtual int  XM_Plugin_initialize(const QString& libname, const QString& classname)=ABSTRACT;
  virtual QString XM_Plugin_call(int id, int fnum, const QString& param)=ABSTRACT;

  // funcs for Connections
  virtual int XM_Connection_initialize(const QString& host, int port, const QString& onConnect="") = ABSTRACT;
  virtual void XM_Connection_open(int id, const QString& host, int port, const QString& onConnect="") = ABSTRACT;
  virtual void XM_Connection_send(int id, const QString& string) = ABSTRACT;
  virtual void XM_Connection_close(int id) = ABSTRACT;
  virtual void XM_Connection_setInputEncoding(int id, const QString& enc) = ABSTRACT;
  virtual void XM_Connection_setOutputEncoding(int id, const QString& enc) = ABSTRACT;

  // funcs for InputLines
  virtual int XM_InputLine_getCursorX(int id) = ABSTRACT;
  virtual int XM_InputLine_getCursorY(int id) = ABSTRACT;
  virtual void XM_InputLine_setCursorX(int id, int x) = ABSTRACT;
  virtual void XM_InputLine_setCursorY(int id, int y) = ABSTRACT;
  virtual QString XM_InputLine_getText(int id) = ABSTRACT;
  virtual void XM_InputLine_setText(int id, const QString& text) = ABSTRACT;

  // global functions
  virtual QString XM_ansiToColored(const QString& string) = ABSTRACT;
  virtual QString XM_ansiToRaw(const QString& string) = ABSTRACT;
  virtual void XM_echoMode(bool state) = ABSTRACT;
  virtual int XM_getActiveWindow() = ABSTRACT;
  virtual int XM_getFocusedInputLine() = ABSTRACT;
  virtual int  XM_mdiWidth() = ABSTRACT;
  virtual int  XM_mdiHeight() = ABSTRACT;
  virtual void XM_playAudio(const QString& filename)=ABSTRACT;
  virtual void XM_resetStatusMsg() = ABSTRACT;
  virtual void XM_send(const QString& string) = ABSTRACT;
  virtual void XM_setStatusMsg(const QString& message) = ABSTRACT;
  virtual void XM_showError(const QString& string) = ABSTRACT;
};

#endif
