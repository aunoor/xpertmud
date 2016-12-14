// -*- c++ -*-
#ifndef SCRIPTING_H
#define SCRIPTING_H

#ifndef ABSTRACT
#  define ABSTRACT 0
#endif

#ifndef NO_KDE
#include "DCOPScripting.h"
#endif

#include <qobject.h>
#include <qstring.h>
#include <qvariant.h>

// #include <string>
// #include <exception>

class GuiScriptingBindings;




// common interface for scripting languages in Xpertmud

class XMScripting: public QObject
// MOC_SKIP_BEGIN
#ifndef NO_KDE
                                 , public ScriptingIface 
#endif
// MOC_SKIP_END
{
  Q_OBJECT
  
public:
  XMScripting(QObject* parent,
	      const char* name);
  virtual ~XMScripting(); // is already virtual through QObject
  
  virtual void setCallback(GuiScriptingBindings *callBack) = ABSTRACT;

  // independent
  virtual void timer() = ABSTRACT;
  virtual void textEntered(const QString & text) = ABSTRACT;
  
  virtual bool keyPressed(const QString & key, 
			  const QString & ascii) = ABSTRACT;

  // connection stuff
  virtual void textReceived(const QString & line, int id) = ABSTRACT;
  virtual void echo(bool state, int id) = ABSTRACT;
  virtual void connectionEstablished(int id) = ABSTRACT;
  virtual void connectionDropped(int id) = ABSTRACT;

  // widget stuff
  virtual void mouseDown(int id, int x, int y) = ABSTRACT;
  virtual void mouseDrag(int id, int x, int y) = ABSTRACT;
  virtual void mouseUp(int id, int x, int y) = ABSTRACT;

  // plugin stuff
  virtual QVariant pluginCall(int id, int function, 
			      const QVariant & args) = ABSTRACT;

  // dcop stuff
  //  virtual void textReceived(const QString& line, int id) {
  //    textReceived(std::string(line.latin1()), id);
  //  }

};

#endif
