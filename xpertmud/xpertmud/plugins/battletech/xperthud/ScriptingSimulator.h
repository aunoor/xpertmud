#ifndef HUD_SCRIPTING_SIMULATOR_H
#define HUD_SCRIPTING_SIMULATOR_H

#include <qobject.h>
#include <qvariant.h>

// Simulate Scripting Plugin Iface for Standalone App
// Could/should be rewritten for WIN32 so that this uses
// the ActiveX/COM interfaces used to interface common
// Windows MUD clients.

// This is the UNIX version that communicates using STDIN/OUT

class QTextStream;

class ScriptingSimulator: public QObject {
  Q_OBJECT
public:

  ScriptingSimulator();
  ~ScriptingSimulator();
public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);

protected slots:
  void slotInputAvailable();


signals:
  void callback(int func, const QVariant & args, QVariant & result);

  
protected:
  QTextStream * input;
  QTextStream * output;
};


#endif
