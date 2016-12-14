// -*- c++ -*-
#ifndef CONNECTION_H
#define CONNECTION_H

#include <qobject.h>
#include <kextendedsocket.h>
#include <qstring.h>

#include <string>
#include <vector>

#include "InputStreamHandler.h"
#include "CharSetParser.h"

class QTextEncoder;

class Connection: public QObject, public InputStreamFilter, public SenderIface {
  Q_OBJECT
public:
  Connection(QWidget *parent, int id, const QString& onConnect);
  virtual ~Connection();
  bool operator==(const Connection& c);
  
public:
  void addInputStreamFilter(InputStreamFilter *inputStreamFilter);
  void parse(const std::string& str);
  bool isConnected();
  QString getHost();
  int getPort();
  int getID();
  
public slots:
  void connectToHost(const QString& host, int port);
  void close();
  void setInputEncoding(const QString& enc);
  void setOutputEncoding(const QString& enc);


  // send stuff, every byte 255 will be 255 255 escaped
  void send(const QString& text);

protected: // SenderIface Impl
  // send the byte without 255 escaping
  void sendByte(char byte);
  // send stuff, every byte 255 will be 255 255 escaped
  void send(const std::string& text);
  void send(const char *text, int length);

	     
signals:
  //   received(originally encoded byte sequence,
  //            encoding of original byte sequence as QString (for example utf-8),
  //            chunk encoded as utf16,
  //            id of sender)
  //  void received(const string&, const QString&, const QString&, int);

  //   just the utf16 encoded string... above version would only make sense
  //   for perl, where it'd take away one encoding step.
  //   let's first see, if it really makes a difference... :-)
  void received(const QString&, int); // text, id

  void connected(int); // id
  void connectionClosed(int); // id
  void echoChanged(bool, int); // id
  void bytesWritten(int, int); // nr, id
  void bytesReceived(int, int); // nr, id
  
private slots:
  void slotSocketReady();
  void slotConnected();
  void slotConnectionClosed();
  void slotError(int);
  void slotEchoChanged(bool);

private:
  typedef std::vector<InputStreamFilter *> filterListT;
  filterListT filters;
  int id;
  KExtendedSocket socket;
  int iBytesWritten;
  int iBytesRead;
  QString onConnect;

  QTextDecoder* textDecoder;
  QTextEncoder* textEncoder;
  QString inputEncoding;
  QString outputEncoding;
  static const QString defaultInputEncoding;
  static const QString defaultOutputEncoding;

  CharSetParser charSetParser;
  
};

#endif
