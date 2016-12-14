#include "ConnectionHandler.h"
#include "TelnetFilter.h"
#include "IacHandlerCollection.h"

#ifndef Q_WS_WIN
#include "MCCP.h"
#endif

#include <qwidget.h>

// TODO: use KDEBUG
#include <iostream>
using std::cout;
using std::endl;

ConnectionHandler::ConnectionHandler(QWidget *parent):
  QObject(parent) {
}

ConnectionHandler::~ConnectionHandler() {
  for(connectionListT::iterator it=connections.begin();
      it != connections.end(); ++it) {
    delete (*it).second;
  }
}

void ConnectionHandler::reset() {
  for(connectionListT::iterator it=connections.begin();
      it != connections.end(); ++it) {
    delete (*it).second;
  }
  connections.clear();
}

int ConnectionHandler::open(const QString& host, int port,
			     const QString& onConnect,
			     QObject *callback,
			     const char *slotConnected,
			     const char *slotConnectionClosed,
			     const char *slotReceived,
			     const char *slotEchoChanged,
			     const char *slotBytesReceived,
			     const char *slotBytesWritten) {
  int id = 0;
  bool found = false;
  do {
    connectionListT::iterator it = connections.find(id);
    if(it == connections.end())
      found = true;
    ++id;
  } while(!found);
  --id;

  open(id, host, port, onConnect, callback, slotConnected,
       slotConnectionClosed, slotReceived, slotEchoChanged,
       slotBytesReceived, slotBytesWritten);

  return id;
}

void ConnectionHandler::open(int id, const QString& host, int port,
			     const QString& onConnect,
			     QObject *callback,
			     const char *slotConnected,
			     const char *slotConnectionClosed,
			     const char *slotReceived,
			     const char *slotEchoChanged,
			     const char *slotBytesReceived,
			     const char *slotBytesWritten,
			     const QString & encoding) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    delete (*it).second;
  }
  Connection *c = new Connection((QWidget*)parent(), id, onConnect);
  connections[id] = c;

  if (encoding.length()) {
      c->setInputEncoding(encoding);
      c->setOutputEncoding(encoding);
  }

  TelnetFilter *telnetFilter = new TelnetFilter(c);

  IacHandlerCollection *collection = new IacHandlerCollection(c, c);
  collection->registerTType("xpertmud");
  collection->registerTType("ansi");
  collection->plug(*telnetFilter);

#ifndef Q_WS_WIN
  MCCP* mccp = new MCCP(c, c);
  mccp->plug(*telnetFilter);
  c->addInputStreamFilter(mccp);
#endif

  c->addInputStreamFilter(telnetFilter);

  if(slotConnected != NULL)
    connect(c, SIGNAL(connected(int)), callback, slotConnected);
  if(slotConnectionClosed != NULL)
    connect(c, SIGNAL(connectionClosed(int)), callback, slotConnectionClosed);
  if(slotReceived != NULL)
    connect(c, SIGNAL(received(const QString&, int)), 
	    callback, slotReceived);
  if(slotEchoChanged != NULL) {
    connect(collection, SIGNAL(echoChanged(bool)),
	    c, SLOT(slotEchoChanged(bool)));
    connect(c, SIGNAL(echoChanged(bool, int)),
	    callback, slotEchoChanged);
  }
  
  if(slotBytesReceived != NULL) {
    connect(c, SIGNAL(bytesReceived(int, int)),
	    callback, slotBytesReceived);
  }
  if(slotBytesWritten != NULL) {
    connect(c, SIGNAL(bytesWritten(int, int)),
	    callback, slotBytesWritten);
  }

  c->connectToHost(host, port);
}

void ConnectionHandler::close(int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->close();
  }
}

void ConnectionHandler::setInputEncoding(const QString& enc, int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->setInputEncoding(enc);
  }
}

void ConnectionHandler::setOutputEncoding(const QString& enc, int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->setOutputEncoding(enc);
  }
}

void ConnectionHandler::send(const QString& text, int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->send(text);
  } else {
    cout << "SEND (" << id << "): " << text << endl;
  }
}

/*
void ConnectionHandler::send(const std::string& text, int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->send(text);
  } else {
    cout << "SEND (" << id << "): " << text << endl;
  }
}

void ConnectionHandler::send(const char *text, int length, int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    (*it).second->send(text, length);
  } else {
    QString toSend;
    for(int i=0; i<length; ++i) toSend += text[i];
    cout << "SEND (" << id << "): " << toSend.latin1() << endl;
  }
}
*/
Connection *ConnectionHandler::get(int id) {
  connectionListT::iterator it = connections.find(id);
  if(it != connections.end()) {
    return (*it).second;
  }
  return NULL;
}

int ConnectionHandler::countConnected() {
  int count = 0;
  for(connectionListT::iterator it=connections.begin();
      it != connections.end(); ++it) {
    if((*it).second->isConnected()) { ++count; }
  }
  return count;
}

list<ConnectionListItem> ConnectionHandler::listConnected() {
  list<ConnectionListItem> cons;
  for(connectionListT::iterator it=connections.begin();
      it != connections.end(); ++it) {
    if((*it).second->isConnected()) { 
      cons.push_back(ConnectionListItem((*it).second->getHost(),
					(*it).second->getPort(),
					(*it).second->getID()));
    }
  }
  return cons;
}
