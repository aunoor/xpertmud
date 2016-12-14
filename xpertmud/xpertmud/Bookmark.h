// -*- c++ -*-
/***************************************************************************
   $Header: /cvsroot/xpertmud/xpertmud/xpertmud/xpertmud.h,v 1.70 2002/08/09 14:
                             -------------------
    begin                :  10.08.2002 13:00 MEST 2002
    copyright            : (C) 2001,2002 by Ernst Bachmann, Manuel Klimek

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <qobject.h>
#include <qstring.h>
#include <qdom.h>

#include <kurl.h>
#include <qvaluevector.h>


// Most of those get-Funcs are "logically" const,
// but can't really be, as they might add some missing
// Nodes, thus modifying the bookmark

// All those setConnectionXXX funcs silently do nothing
// if the specified cid was not found, so check for existence first

class Bookmark:public QObject {
  Q_OBJECT

public:
  // construct empty bookmark;
  Bookmark();
  Bookmark(const Bookmark &);
  const Bookmark & operator=(const Bookmark &);

  // File operations, return value indicates success
  bool load(const KURL & url);
  bool save(const KURL & url);

  // File handling related stuff
  bool edited() const { return isEdited; }
  void setEdited();

  // returns a nice string suitable as title
  // '*' is appended if Modified
  // 'unnamed' is returned for new Bookmarks
  QString getTitle() const;

  
  // setting the url is only possible via load/save...
  const KURL& url() const { return bookmark_name; }

  // General Settings stuff
  QString getName();
  QString getComments();

  // TODO: Icon, ???

  // Scripting settings
  bool getStartInterpreter();
  QString getInterpreter();
  bool getResetInterpreter();
  QString getGlobalScript();

  // Connection Stuff
#ifdef USE_STL_VECTOR_FOR_CONNECTIONS
  vector<int> getAvailableConnections();
#else
  QValueVector<int> getAvailableConnections();
#endif
  bool isConnectionAvailable(int);
  void addConnection(int);
  void removeConnection(int);
  void renameConnection(int oldID, int newID);
  QString getConnectionHost(int);
  void setConnectionHost(int, const QString &);
  QString getConnectionEncoding(int);
  void setConnectionEncoding(int, const QString &);
  short unsigned int getConnectionPort(int);
  void setConnectionPort(int, unsigned short int);
  QString getConnectionScript(int);
  void setConnectionScript(int, const QString &);

  // for script-stored configuration values
  void setScriptConfig(const QString & key, const QString & value);
  QString getScriptConfig(const QString & key);

public slots:
  void setName(const QString &); 
  void setComments(const QString &);
  void setStartInterpreter(bool);
  void setInterpreter(const QString &);
  void setResetInterpreter(bool);
  void setGlobalScript(const QString &);

signals:
  // TODO: Use KMainWindows setCaption(string,bool) for this
  void titleChanged(const QString & title);

protected:
  QDomElement getNamedElement(const QString & name);
  QString getNamedTopNode(const QString & name);
  void setNamedTopNode(const QString & name,const QString & value, bool cdata=false);
  QDomElement getConnectionElement(int);
protected:
  bool isEdited;
  KURL bookmark_name;
  QDomDocument data;
};

#endif
