/***************************************************************************
 *
 * $Id: InputAction.cc 1140 2003-11-15 17:05:49Z entropy $
 *
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "InputAction.h"
#include "mxinputline.h"
#include "RegisterIface.h"

InputAction::InputAction( const QString& text, int accel, 
			  const QObject *receiver,
			  const QString & slotTextEntered, 
			  const QString & signalEchoMode, 
			  const QString & signalCommandRetention,
			  QObject* parent, const char* name,
			  RegisterIface* registrar):
  KAction(text,accel,parent,name),
  receiver(receiver),
  slotTextEntered(slotTextEntered),
  signalEchoMode(signalEchoMode),
  signalCommandRetention(signalCommandRetention),
  registrar(registrar)
{}

int InputAction::plug( QWidget *w, int index ) {

  KToolBar *toolBar = (KToolBar *)w;
  
  int id = KAction::getToolButtonID();

  MXInputLine * inputline = new MXInputLine("", toolBar);
  inputline->setID(registrar->registerWidget(inputline));
  //  inputline->setTrapReturnKey(false);
  inputline->setFocus();
  toolBar->insertWidget( id, 70, inputline, index );
  toolBar->setItemAutoSized( id, true );

  connect(inputline, SIGNAL(textEntered(const QString&)),
  	  receiver, slotTextEntered);
  connect(receiver, signalEchoMode,
	  inputline, SLOT(slotEchoMode(bool)));
  connect(receiver, signalCommandRetention,
	  inputline, SLOT(slotCommandRetention(bool)));

  connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

  QWhatsThis::add( inputline, whatsThis() );
  
  return containerCount() - 1;
}

void InputAction::unplug( QWidget *w ) {
  registrar->unregisterWidget(w);

  KToolBar *toolBar = (KToolBar *)w;
  int idx = findContainer( w );

  toolBar->removeItem( itemId( idx ) );

  removeContainer( idx );
}
