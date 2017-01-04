#include "PluginWrapper.h"

PluginWrapper::PluginWrapper(QObject * plugin, int id):
  QObject(plugin),pluginID(id) {
  
  connect(plugin,SIGNAL(callback(int, const QVariant &, QVariant &)),
	  this,SLOT(slotCallback(int, const QVariant &, QVariant &)));
  
  connect(this,SIGNAL(functionCall(int, const QVariant &, QVariant &)),
	  plugin,SLOT(slotFunctionCall(int, const QVariant &, QVariant &)));

}

void PluginWrapper::slotFunctionCall(int fid, const QVariant & args, QVariant & result) {
  emit functionCall(fid,args,result);
}

void PluginWrapper::slotCallback(int fid, const QVariant & args, QVariant & result) {
  emit callback(pluginID,fid,args,result);
}

