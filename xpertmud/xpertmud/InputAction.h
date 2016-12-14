// -*- c++ -*-
/***************************************************************************
 *
 * $Id: InputAction.h 1140 2003-11-15 17:05:49Z entropy $
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
#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include <qwhatsthis.h>

#include <kaction.h>
#include <ktoolbar.h>

class MXInputLine;
class RegisterIface;

class InputAction : public KAction {
  Q_OBJECT
public:
  InputAction( const QString& text, int accel, const QObject *receiver, 
	       const QString & slotTextEntered, 
	       const QString & signalEchoMode, 
	       const QString & signalCommandRetention,
	       QObject* parent, const char* name,
	       RegisterIface* registrar);
  

  virtual int plug( QWidget *w, int index = -1 );
  
  virtual void unplug( QWidget *w );
  
private:
  const QObject *receiver;
  // TODO: give them real names, move to QString
  QString slotTextEntered;
  QString signalEchoMode;
  QString signalCommandRetention;
  QString slotTextCompletion;

  RegisterIface* registrar;
};  

#endif
