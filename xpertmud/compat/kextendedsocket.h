// -*- c++ -*-
#ifndef KEXTENDEDSOCKET_H
#define KEXTENDEDSOCKET_H

#include <QTcpSocket>
#include <QTextStream>
#include <iostream>
using std::cout;
using std::endl;

class KExtendedSocket;

class QSocketWrapper: public QTcpSocket {
  Q_OBJECT
public:
  QSocketWrapper(): QTcpSocket() {}
  virtual ~QSocketWrapper() {}
  
  friend class KExtendedSocket;
};

class KExtendedSocket: public QObject {
  Q_OBJECT
public:
  enum DontKnowName {
    connected,
    unknown
  };

  enum DontKnowToo {
    inetSocket,
    inputBufferedSocket,
    streamSocket
  };
  
  KExtendedSocket() {
    connect(&socket, SIGNAL(readyRead()),
	    this, SLOT(slotReadyRead()));
    connect(&socket, SIGNAL(connected()),
	    this, SLOT(slotConnected()));
    connect(&socket, SIGNAL(connectionClosed()),
	    this, SLOT(slotConnectionClosed()));
    connect(&socket, SIGNAL(error(int)),
	    this, SLOT(slotError(int)));
  }

  QString host() {
    return socket.peerName();
  }
  QString port() {
    return QString::number(socket.peerPort());
  }

  void setSocketFlags(int) {}
  void setHost(const QString& host) {
    _host = host;
  }
  void setPort(int port) {
    _port = port;
  }

  void setTimeout(int) {}
  void enableRead(bool) {}
  int lookup() { return 0; }
  int startAsyncConnect() {
    cout << "Connecting to: " << _host.toLatin1().data() << ", " << _port << endl;

    socket.connectToHost(_host, _port);
    return 0;
  }
  void close() {
    socket.close();
  }
  void writeBlock(const char *block, int length) {
    socket.write(block, length);
  }
  int readBlock(char *block, int maxlen) {
    return (int)socket.read(block, maxlen);
  }
  int bytesAvailable() {
    return (int)socket.bytesAvailable();
  }
  int status() {
    return err;
  }
  int systemError() {
    return 0;
  }
  static QString strError(int err, int) {
    return "Error (TODO: specify)" +
      QString::number(err);
  }
  
  int state() {
    if(socket.state() == QTcpSocket::ConnectedState) {
      return connected;
    }
    return unknown;
  }

signals:
  void readyRead(); 
  void connectionSuccess();
  void closed(int);
  void connectionFailed(int);

private slots:
  void slotReadyRead() {
    while(bytesAvailable() > 0)
      emit readyRead();
  }
  void slotConnected() {
    emit connectionSuccess();
  }
  void slotConnectionCLosed() {
    emit closed(0);
  }
  void slotError(int e) {
    err = e;
    emit connectionFailed(e);
  }

protected:
  QTcpSocket socket;
  QString _host;
  int _port;
  int err;
};

#endif
