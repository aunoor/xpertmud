// -*- c++ -*-
/***************************************************************************
   $Header$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PLUGIN_WRAPPER_H
#define PLUGIN_WRAPPER_H

#include <qobject.h>
#include <qvariant.h>

class PluginWrapper:public QObject {
  Q_OBJECT
public:
  PluginWrapper(QObject * plugin, int id);

public slots:
  void slotFunctionCall(int fid, const QVariant & args, QVariant & result);
  void slotCallback(int fid, const QVariant & args, QVariant & result); 
  
signals:
  void functionCall(int fid, const QVariant & args, QVariant & result);
  void callback(int pluginID, int fid, const QVariant & args, QVariant & result);

protected:
  int pluginID;
};

#endif
