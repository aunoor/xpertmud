#include "Connection.h"

#include <qwidget.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <cassert>
//#include <ksock.h>


#include <iostream>



const QString Connection::defaultInputEncoding = "ISO-8859-1";
const QString Connection::defaultOutputEncoding = "ISO-8859-1";

Connection::Connection(QWidget *parent, int id, const QString& onConnect):
  QObject(parent),
  InputStreamFilter(NULL, NULL), id(id),
  iBytesWritten(0), iBytesRead(0),
  onConnect(onConnect)
{
  inputEncoding = defaultInputEncoding;
  textDecoder = QTextCodec::codecForName(defaultInputEncoding)->makeDecoder();

  outputEncoding = defaultOutputEncoding;
  textEncoder = QTextCodec::codecForName(defaultOutputEncoding)->makeEncoder();

  connect(&socket, SIGNAL(readyRead()), 
	  this, SLOT(slotSocketReady()));
  connect(&socket, SIGNAL(connectionSuccess()), 
	  this, SLOT(slotConnected()));
  connect(&socket, SIGNAL(closed(int)), 
	  this, SLOT(slotConnectionClosed()));
  connect(&socket, SIGNAL(connectionFailed(int)),
	  this, SLOT(slotError(int)));

}

Connection::~Connection() {
  for(filterListT::iterator it=filters.begin();
      it != filters.end(); ++it) {
    delete *it;
  }
  delete textDecoder;
  delete textEncoder;
}

bool Connection::operator==(const Connection& c) {
  return (id == c.id);
}

void Connection::addInputStreamFilter(InputStreamFilter *inputStreamFilter) {
  if(!filters.empty()) {
    (*filters.rbegin())->setNext(inputStreamFilter);
    inputStreamFilter->setFirst(*filters.begin());
  } else {
    inputStreamFilter->setFirst(inputStreamFilter);
  }
  filters.push_back(inputStreamFilter);
  (*filters.rbegin())->setNext(this);
}

void Connection::parse(const std::string& str) {
  QString utf16;
  for(unsigned int i=0; i<str.size(); ++i) {
    // AFAIK: 
    // the character set changes are encoded as ESC 
    // sequences in the encoded string, which is a pain
    // in the ass! So we have to decode the string
    // before we know what character encodings the next
    // bytes are in... really really bad, I'll have to
    // implement a test server for such stuff ;-) to
    // test other clients as well *heh*
    // two possibilities: 
    // - either parse each byte, check the QString for
    //   being empty, if the QString is a QChar long, check
    //   for character set changes, if it's a character
    //   change sequence, create a new QTextDecoder and
    //   go on
    // - parse a whole chunk as QString, then search the
    //   QString for character set changes. If a cs change
    //   occurs, somehow find the byte that caused it and
    //   start encoding from there on. This seems rather
    //   impossible, any ideas?
    // so we go for solution 1, which is probably verra slow
    
    // one byte at a time
    QString utfSeq = textDecoder->toUnicode(&(str.c_str())[i], 1);
    if(!utfSeq.isNull()) {
      // looks like one byte can complete a sequence of
      // multiple characters, so the next line is a no-no
      // assert(oneChar.length() == 1);
      for(unsigned int j=0; j<utfSeq.length(); ++j) {
	if(charSetParser.add(utfSeq.at(j))) { // charset changed
	  setInputEncoding(charSetParser.encoding());
	  /*	  inputEncoding = charSetParser.encoding();
	    QTextCodec* codec = QTextCodec::codecForName(inputEncoding);
	    if(codec == NULL) {
	    inputEncoding = defaultInputEncoding;
	    codec = QTextCodec::codecForName(inputEncoding);
	    }
	    textDecoder = codec->makeDecoder();
	  */
	  
	} else { // charset didnt change
	  if(charSetParser.isIdle()) {
	    utf16 += charSetParser.readUntilIdle();
	  }
	}
      }
    }
  }
  emit received(utf16, id);
}

bool Connection::isConnected() {
  return (socket.state() == KExtendedSocket::connected);
}

QString Connection::getHost() {
  return socket.host();
}

int Connection::getPort() {
  // Bad, Bad C style conversion
  //  return atoi(socket.port().latin1());
  return socket.port().toInt();
}
 
int Connection::getID() {
  return id;
}


void Connection::connectToHost(const QString& host, int port) {
  //  cout << "Connecting to: " << host << ", " << port << endl;
  socket.setSocketFlags(KExtendedSocket::inetSocket 
			//| KExtendedSocket::inputBufferedSocket 
			//| KExtendedSocket::streamSocket
			//| KExtendedSocket::noResolve
			);
  socket.setHost(host);
  socket.setPort(port);
  socket.setTimeout(10);
  socket.enableRead(true);
  if(socket.lookup()) { // without this we get Connection refused on many sites
    // TODO: inform user
    //    cout << "lookup error!" << endl;
  }
  //  cout << "Starting async connect" << endl;
  socket.startAsyncConnect();
  //  cout << "returns: " << err << endl; 
  // TODO: inform user if err
}

void Connection::close() {
  socket.close();
}

void Connection::setInputEncoding(const QString& enc) {
  QTextCodec* codec = QTextCodec::codecForName(enc);
  if(codec != NULL) {
    inputEncoding = enc;
    delete textDecoder;
    textDecoder = codec->makeDecoder();

    setOutputEncoding(enc);
  }
}

void Connection::setOutputEncoding(const QString& enc) {
  QTextCodec* codec = QTextCodec::codecForName(enc);
  if(codec != NULL) {
    outputEncoding = enc;
    delete textEncoder;
    textEncoder = codec->makeEncoder();
  }
}

void Connection::sendByte(char byte) {
  socket.writeBlock(&byte, 1);
  iBytesWritten+=1;
  emit bytesWritten(1, id);
}

void Connection::send(const char *text, int length) {
  // 255 becomes 255 255
  // if every char is 255, the max length is 2*length
  char* copy = new char[2*length];

  int newLength = 0;
  for(int i=0; i<length; ++i, ++newLength) {
    if(text[i] != (char)255) {
      copy[newLength] = text[i];
    } else {
      copy[newLength] = (char)255;
      ++newLength;
      copy[newLength] = (char)255;
    }
  }
  socket.writeBlock(copy, newLength);
  iBytesWritten+=newLength;
  emit bytesWritten(newLength, id);

  delete[] copy;
}


void Connection::send(const QString& text) {
  int length = text.length();
  QCString s = textEncoder->fromUnicode(text, length);
  send(s.data(), length);
}

void Connection::send(const std::string& text) {
  send(text.c_str(), text.length());
}

void Connection::slotSocketReady() {
  const int maxlen = 1024;
  char text[maxlen];

  QTime timer;
  timer.start();
  int readBytes;
  do {
    readBytes = socket.readBlock(text, maxlen);
    //    std::cout << "Read " << readBytes << " bytes" << std::endl;
    if(readBytes == -1) {
      KMessageBox::error((QWidget*)parent(),i18n("Evil error occured :)"), i18n("Error !"));
      socket.close();
      return;
    }
    if (readBytes==0) {
      // socket closed...
      // close it manually so that the scripting engine receives its callback
      socket.close();
      return;
    }
    iBytesRead += readBytes;
    emit bytesReceived(readBytes, id);
    
    //std::cout << "<Elapsed " << timer.elapsed() << std::endl;
    if(!filters.empty()) {
      filters[0]->parse(std::string(text, readBytes));
    } else {
      parse(std::string(text,readBytes));
    }
    //    std::cout << "<Elapsed " << timer.elapsed() << std::endl;
  } while(readBytes == maxlen && socket.bytesAvailable() && timer.elapsed() < 70);
}

void Connection::slotConnected() {
  send(onConnect.latin1(), onConnect.length());

  emit connected(id);
}

void Connection::slotConnectionClosed() {
  emit connectionClosed(id);
}

void Connection::slotError(int /* e */) {
  //  cout << "Error while connecting: " << e << endl;
  QString errorString =
    KExtendedSocket::strError(socket.status(),
			      socket.systemError());

  KMessageBox::error((QWidget*)parent(),
		     i18n("Error In Connection:\n%1, %2\n%3")
		     .arg(socket.host())
		     .arg(socket.port())
		     .arg(errorString),
		       i18n("Error !"));

  slotConnectionClosed();
}

void Connection::slotEchoChanged(bool b) {
  emit echoChanged(b, id);
}
