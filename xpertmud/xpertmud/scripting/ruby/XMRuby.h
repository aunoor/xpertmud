// -*- c++ -*-
#ifndef XM_RUBY_H
#define XM_RUBY_H
#include "../Scripting.h"


class GuiScriptingBindings;



class XMRuby: public XMScripting {

Q_OBJECT

public:
  XMRuby(QObject* parent,
	   const char* name);
  ~XMRuby();
 
  void setCallback(GuiScriptingBindings *callBack);

  void timer();
  void textEntered(const QString & text);
  bool keyPressed(const QString & key, const QString & ascii);

  void textReceived(const QString & line, int id);
  void echo(bool state, int id);
  void connectionEstablished(int id);
  void connectionDropped(int id);

  void mouseDown(int id, int x, int y);
  void mouseDrag(int id, int x, int y);
  void mouseUp(int id, int x, int y);

  QVariant pluginCall(int id, int function, const QVariant & args);

protected:
  void reportError(const QString &,int);
  bool evalSysInit();
  
  GuiScriptingBindings * callBack;
  bool initialized;
};
#endif // #ifndef XM_RUBY_H

