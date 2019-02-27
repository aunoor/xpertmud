// -*- c++ -*-
#ifndef XM_PERL_H
#define XM_PERL_H
#include "../Scripting.h"
#include "AutoGuiScriptingWrapper.h"

// This is a wrapper around PerlInterpret,
// as you cannot include both QT and Perl in one file :(
class GuiScriptingBindings;
class PerlInterpret;

class XMPerl: public XMScripting {

Q_OBJECT

public:
  XMPerl(QObject* parent,
	 const char* name);
  ~XMPerl();

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
  PerlInterpret * interp;
  GuiScriptingWrapper wrapper;
  int recursionCounter;
};
#endif
