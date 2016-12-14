#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>
#include "../BattleCoreWidget.h"
#include "ScriptingSimulator.h"
#include "../battletech.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

static const char *description =
	I18N_NOOP("XpertHUD");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

static KCmdLineOptions options[] =
{
  //  { "+[xmud]", I18N_NOOP("The xpertmud bookmark file to open"), 0 },
  // INSERT YOUR COMMANDLINE OPTIONS HERE
  { 0, 0, 0 }
};



int main(int argc, char *argv[]) {

  KAboutData aboutData( "xperthud", I18N_NOOP("XpertHUD"),
			VERSION, description, KAboutData::License_GPL,
			"(c) 2001,2002 Ernst Bachmann & Manuel Klimek", 0, 0, 
			"e.bachmann@xebec.de"); // TODO: fill in addr for bugreports, etc
  aboutData.addAuthor("Manuel Klimek",0, "manuel.klimek@web.de");
  aboutData.addAuthor("Ernst Bachmann",0, "e.bachmann@xebec.de");
  
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  
  KApplication a;

  ScriptingSimulator sim;

  BattleCoreWidget * bcw=new BattleCoreWidget(NULL,"BattleCoreWidget",QStringList());
  a.setMainWidget(bcw);
  bcw->show();
  BattleMapView * bmw=new BattleMapView(NULL,"BattleMapView");
  bmw->show();

  a.connect(bcw,SIGNAL(callback(int, const QVariant &, QVariant &)),
	    &sim,SLOT(slotFunctionCall(int, const QVariant &, QVariant &)));

  a.connect(&sim,SIGNAL(callback(int, const QVariant &, QVariant &)),
	    bcw,SLOT(slotFunctionCall(int, const QVariant &, QVariant &)));

  return a.exec();
}
