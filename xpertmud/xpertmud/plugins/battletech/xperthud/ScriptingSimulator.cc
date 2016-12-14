#include "ScriptingSimulator.h"
#include <qsocketnotifier.h> 


ScriptingSimulator::ScriptingSimulator() {
  input=new QTextStream(stdin,  IO_ReadOnly );
  output=new QTextStream( stdout, IO_WriteOnly );
  QSocketNotifier * notifier=new QSocketNotifier(0,QSocketNotifier::Read,this);
  
  connect(notifier,SIGNAL(activated(int)),
	  this,SLOT(slotInputAvailable()));
}
ScriptingSimulator::~ScriptingSimulator() {
  // Hope this doesnt close the handles....
  delete input;
  delete output;
}

void ScriptingSimulator::slotInputAvailable() {
  // TODO: this might block till \n is read..
  QString line=input->readLine();
  QVariant result;
  emit callback(0,QVariant(line),result);
}

void ScriptingSimulator::slotFunctionCall(int func, 
					  const QVariant & args, 
					  QVariant & result) {
  if (func==0) {
    *output << args.toString();
  }

}




