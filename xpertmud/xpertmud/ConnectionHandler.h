// -*- c++ -*-
#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include "Connection.h"

#include <qobject.h>

#include <list>
#include <map>
using std::list;
using std::map;

class ConnectionListItem {
public:
  ConnectionListItem(const QString& host, int port, int id):
    host(host), port(port), id(id) {}
  QString host;
  int port;
  int id;
};

class ConnectionHandler: public QObject {
  Q_OBJECT

  typedef map<int, Connection*> connectionListT;
public:
  ConnectionHandler(QWidget *parent);
  virtual ~ConnectionHandler();

  /**
   * Close and delete all connections.
   */
  void reset();

  int open(const QString& host, int port,
	    const QString& onConnect,
	    QObject *callback,
	    const char *slotConnected = NULL,
	    const char *slotConnectionClosed = NULL,
	    const char *slotReceived = NULL,
	    const char *slotEchoChanged = NULL,
	    const char *slotBytesReceived = NULL,
	    const char *slotBytesWritten = NULL);

  void open(int id, const QString& host, int port,
	    const QString& onConnect,
	    QObject *callback,
	    const char *slotConnected = NULL,
	    const char *slotConnectionClosed = NULL,
	    const char *slotReceived = NULL,
	    const char *slotEchoChanged = NULL,
	    const char *slotBytesReceived = NULL,
	    const char *slotBytesWritten = NULL,
	    const QString & encoding = QString());
  
  void close(int id);
  void setInputEncoding(const QString& enc, int id);
  void setOutputEncoding(const QString& enc, int id);
//  void send(const std::string& text, int id);
//  void send(const char* text, int length, int id);
  void send(const QString& text, int id);

  /**
   * @return NULL, if no connection available
   */
  Connection *get(int id);

  int countConnected();
  list<ConnectionListItem> listConnected();
  
private:
  connectionListT connections;
};

#endif
