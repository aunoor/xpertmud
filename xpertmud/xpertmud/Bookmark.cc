/***************************************************************************
   $Header: /cvsroot/xpertmud/xpertmud/xpertmud/xpertmud.h,v 1.70 2002/08/09 14:
                             -------------------
    begin                : 10.08.2002 13:00 MEST 2002
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

#include "Bookmark.h"
#include <qfile.h>
#include <qtextstream.h>
#include <klocale.h>
#include <kio/netaccess.h>

#include <cassert>

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define VERSION "win32-unknown"
#endif


Bookmark::Bookmark():
isEdited(false),data("xpertmudBookmark"){
  QDomElement root = data.createElement("xpertmudBookmark");
  root.setAttribute("version",VERSION);
  data.appendChild( root );
}


Bookmark::Bookmark(const Bookmark & o):
  QObject(),// explicitly no call to QObject copy ctor
  isEdited(o.isEdited),bookmark_name(o.bookmark_name)
{

  data=o.data.cloneNode().toDocument();
  assert(!data.isNull());
  QDomElement e=data.documentElement();
  assert(!e.isNull());
  assert(e.tagName()=="xpertmudBookmark");
}


const Bookmark & Bookmark::operator=(const Bookmark & o) {
 // explicitly no call to QObject assignment op.
  isEdited=o.isEdited;
  bookmark_name=o.bookmark_name;

  data=o.data.cloneNode().toDocument();
  assert(!data.isNull());
  QDomElement e=data.documentElement();
  assert(!e.isNull());
  assert(e.tagName()=="xpertmudBookmark");

  emit titleChanged(getTitle());

  // TODO! Correct?
  return *this;
}


bool Bookmark::load(const KURL & url) {
  QString tmpfile;
  if (KIO::NetAccess::download( url, tmpfile )) {
    QDomDocument doc( "xpertmudBookmark" );
    QFile f( tmpfile );
    if ( !f.open( IO_ReadOnly ) ) {
      KIO::NetAccess::removeTempFile( tmpfile );
      //      KMessageBox::error(this,i18n("Bookmark '%1' could not be read").arg(url.url()), i18n("Error !"));
      return false;
    }
    if ( !doc.setContent( &f ) ) {
      f.close();
      KIO::NetAccess::removeTempFile( tmpfile );
      //   KMessageBox::error(this,i18n("Bookmark '%1' could not be parsed").arg(url.url()), i18n("Error !"));
      return false;
    }
    f.close();
    KIO::NetAccess::removeTempFile( tmpfile );
    bookmark_name=url;
    isEdited=false;
    data=doc;
    if (data.documentElement().isNull()) {
      QDomElement root = data.createElement("xpertmudBookmark");
      root.setAttribute("version",VERSION);
      data.appendChild( root );
    }
    emit titleChanged(getTitle());
    return true;
  }
  //    KMessageBox::error(this,i18n("Bookmark '%1' could not be opened").arg(url.url()), i18n("Error !"));
  return false;
}


bool Bookmark::save(const KURL & url) {
  if (!url.isLocalFile()) {
    //    KMessageBox::error(this,i18n("Sorry, only saving to local files implemented"));
    return false;
  }
  QFile f(url.directory()+'/' +url.fileName());
  if (!f.open(IO_WriteOnly)) {
    //    KMessageBox::error(kapp,i18n("Could not open %1 for writing").arg(url.fileName()));
    return false;
  }
  { // scope for fs (delete before f)
    QTextStream fs(&f);
    data.save(fs,4);
  }
  f.close();
  

  isEdited=false;
  bookmark_name=url;
  emit titleChanged(getTitle());
  return true;

}

QString Bookmark::getTitle() const {
  QString title;
  if (url().isEmpty())
    title=i18n("unnamed");
  else
    title=url().fileName();

  if (edited())
    title+=" *"; // " (modified)" ????
  
  

  return title;
}

void Bookmark::setEdited() {
  if (!edited()) {
    isEdited=true;
    emit titleChanged(getTitle());
  }
}

QString Bookmark::getName() {
  return getNamedTopNode("Name");
}


void Bookmark::setName(const QString & name) {
  setNamedTopNode("Name",name);
}

 
QString Bookmark::getComments() {
  return getNamedTopNode("Comments");
}


void Bookmark::setComments(const QString & com) {
  setNamedTopNode("Comments",com,true);
}



bool Bookmark::getStartInterpreter() {
  return getNamedElement("Interpreter").attribute("startup")=="true";
}


void Bookmark::setStartInterpreter(bool start) {
  getNamedElement("Interpreter").setAttribute("startup",start?"true":"false");
  setEdited();
}


QString Bookmark::getInterpreter() {
  return getNamedTopNode("Interpreter");
}


void Bookmark::setInterpreter(const QString & intp) {
  setNamedTopNode("Interpreter",intp);
  setEdited();
}


bool Bookmark::getResetInterpreter() {
  return getNamedElement("Interpreter").attribute("reset")=="true";
}


void Bookmark::setResetInterpreter(bool reset) {
  getNamedElement("Interpreter").setAttribute("reset",reset?"true":"false");
  setEdited();
}


QString Bookmark::getGlobalScript() {
  return getNamedTopNode("GlobalScript");
}


void Bookmark::setGlobalScript(const QString & script) {
  setNamedTopNode("GlobalScript",script,true);
}


QValueVector<int> Bookmark::getAvailableConnections() {
  QValueVector<int> cids;

  QDomNodeList conns=data.elementsByTagName("Connection");
  for (unsigned int i=0; i<conns.count(); ++i) {
    QDomNode n = conns.item(i);
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      QString id=e.attribute("id",QString::number(i));
      bool ok=false;
      int nid=id.toInt(&ok);
      if (ok) {
	cids.push_back(nid);
      }
    }
  }
  return cids;
}

bool Bookmark::isConnectionAvailable(int cid) {
  return !getConnectionElement(cid).isNull();
}


void Bookmark::addConnection(int cid) {
  QDomElement e=data.createElement("Connection");
  e.setAttribute("id",QString::number(cid));
  data.documentElement().appendChild(e);
  setEdited();
}


void Bookmark::removeConnection(int cid) {
  QDomElement e=getConnectionElement(cid);
  if (!e.isNull()) {
    data.documentElement().removeChild(e);
    setEdited();
  }
}


void Bookmark::renameConnection(int oldID, int newID) {
  QDomElement e=getConnectionElement(oldID);
  if (!e.isNull()) {
    e.setAttribute("id",QString::number(newID));
    setEdited();
  }
}


QString Bookmark::getConnectionHost(int cid) {
  QDomElement e=getConnectionElement(cid);
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Host");
    QDomElement h=x.toElement();
    if (!h.isNull()) {
      return h.text();
    } else {
      h=data.createElement("Host");
      e.appendChild(h);
      setEdited();
      return h.text();
    }
  }
  return "";
}


void Bookmark::setConnectionHost(int cid, const QString & host) {
  QDomElement e=getConnectionElement(cid);
  QDomElement h;
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Host");
    h=x.toElement();
    if (h.isNull()) {
      h=data.createElement("Host");
      e.appendChild(h);
    }
    //    while (h.hasChildNodes()) { 
    while (!h.firstChild().isNull()) { // QT2 fix FIX TODO! this
      h.removeChild(h.firstChild());
    }
    h.appendChild(data.createTextNode(host));
    setEdited();
  }
}

QString Bookmark::getConnectionEncoding(int cid) {
  QDomElement e=getConnectionElement(cid);
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Encoding");
    QDomElement h=x.toElement();
    if (!h.isNull()) {
      return h.text();
    } else {
      h=data.createElement("Encoding");
      e.appendChild(h);
      setEdited();
      return h.text();
    }
  }
  return "";
}


void Bookmark::setConnectionEncoding(int cid, const QString & enc) {
  QDomElement e=getConnectionElement(cid);
  QDomElement h;
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Encoding");
    h=x.toElement();
    if (h.isNull()) {
      h=data.createElement("Encoding");
      e.appendChild(h);
    }
    //    while (h.hasChildNodes()) { 
    while (!h.firstChild().isNull()) { // QT2 fix FIX TODO! this
      h.removeChild(h.firstChild());
    }
    h.appendChild(data.createTextNode(enc));
    setEdited();
  }
}


short unsigned int Bookmark::getConnectionPort(int cid) {
  QDomElement e=getConnectionElement(cid);
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Port");
    QDomElement h=x.toElement();
    if (!h.isNull()) {
      return h.text().toUShort();
    } else {
      h=data.createElement("Port");
      e.appendChild(h);
      setEdited();
      return 0;
    }
  }
  return 0;
}


void Bookmark::setConnectionPort(int cid, unsigned short int port) {
  QDomElement e=getConnectionElement(cid);
  QDomElement h;
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Port");
    h=x.toElement();
    if (h.isNull()) {
      h=data.createElement("Port");
      e.appendChild(h);
    }
    //    while (h.hasChildNodes()) { // IS THIS CORRECT?????
    while (!h.firstChild().isNull()) { 
      h.removeChild(h.firstChild()); // TODO: Correct?
    }
    h.appendChild(data.createTextNode(QString::number(port)));
    setEdited();
  }
}


QString Bookmark::getConnectionScript(int cid)  {
  QDomElement e=getConnectionElement(cid);
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Script");
    QDomElement h=x.toElement();
    if (!h.isNull()) {
      return h.text();
    } else {
      h=data.createElement("Script");
      e.appendChild(h);
      setEdited();
      return h.text();
    }
  }
  return "";
}


void Bookmark::setConnectionScript(int cid, const QString & script) {
  QDomElement e=getConnectionElement(cid);
  QDomElement h;
  if (!e.isNull()) {
    QDomNode x=e.namedItem("Script");
    h=x.toElement();
    if (h.isNull()) {
      h=data.createElement("Script");
      e.appendChild(h);
    }
    //while (h.hasChildNodes()) {
    while (!h.firstChild().isNull()) { // TODO: Check
      h.removeChild(h.firstChild());  // dito
    }
    h.appendChild(data.createCDATASection(script));
    setEdited();
  }
}



QDomElement Bookmark::getNamedElement(const QString & name) {
  QDomNodeList conns=data.elementsByTagName(name);
  for (unsigned int i=0; i<conns.count(); ++i) {
    QDomNode n = conns.item(i);
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      return e;
    }
  }
  QDomElement e=data.createElement(name);
  data.documentElement().appendChild(e);
  setEdited();
  return e;
}

QString Bookmark::getNamedTopNode(const QString & name) {
  return getNamedElement(name).text();
}

void Bookmark::setNamedTopNode(const QString & name,const QString & value, bool cdata) {
  bool set=false;
  QDomNode content;
  if (cdata)
    content=data.createCDATASection(value);
  else
    content=data.createTextNode(value);

  QDomNodeList conns=data.elementsByTagName(name);
  for (unsigned int i=0; i<conns.count(); ++i) {
    QDomNode n = conns.item(i);
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      // while (e.hasChildNodes()) {
      while (!e.firstChild().isNull()) {
	e.removeChild(e.firstChild());
      }
      e.appendChild(content);
      set=true;
    }
  }
  if (!set) {
    QDomElement e=data.createElement(name);
    e.appendChild(content);
    data.documentElement().appendChild(e);
  }
  setEdited();
}

QDomElement Bookmark::getConnectionElement(int cid) {
  QDomNodeList conns=data.elementsByTagName("Connection");
  for (unsigned int i=0; i<conns.count(); ++i) {
    QDomNode n = conns.item(i);
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      QString id=e.attribute("id",QString::number(i));
      bool ok=false;
      int nid=id.toInt(&ok);
      if (ok && nid==cid) {
	return e;
      }
    }
  }
  return QDomElement();
}
