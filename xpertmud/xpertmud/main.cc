/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Jun 19 11:36:10 MEST 2001
    copyright            : (C) 2001 by Ernst Bachmann
    email                : e.bachmann@xebec.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "xpertmud.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LogToFile.h"

LogToFile	*myLog = 0;

static const char *description =
	I18N_NOOP("Xpertmud");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

	
static KCmdLineOptions options[] =
{
  { "+[xmud]", I18N_NOOP("The xpertmud bookmark file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	myLog = new LogToFile (1);

  KAboutData aboutData( "xpertmud", I18N_NOOP("Xpertmud"),
			VERSION, description, KAboutData::License_GPL,
			"(c) 2001,2002,2003 Ernst Bachmann & Manuel Klimek\n"
			"(c) 2003-2006 The xpertmud developers", 0, 0, 
			0); // TODO: fill in addr for bugreports, etc

  aboutData.addAuthor("Manuel Klimek",0, "klimek@box4.net");
  aboutData.addAuthor("Ernst Bachmann",0, "e.bachmann@xebec.de");

 // TODO: get peters new email address.
  aboutData.addAuthor("Peter Triller",0, "ptriller@xebec.de");
 // TODO: add other Authors here as well.

  /*
    // We don't have a feedback dialog any more
  aboutData.addCredit( "Stefan Hundhammer",
		       I18N_NOOP("the feedback mail dialog"),
		       "sh@suse.de", "http://www.suse.de/~sh/" );
  */
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;
  Xpertmud *xpertmud = new Xpertmud();
  //a.setMainWidget(xpertmud);
  xpertmud->show();  
  xpertmud->initialize();
  if (a.isRestored() && KMainWindow::canBeRestored(1)) {
    xpertmud->restore(1);
  } else {

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if ( args->count() ) {
      // we only process the first arg
      xpertmud->slotLoadBookmark(args->arg( 0 ));
    
    }
  }
  return a.exec();
}
