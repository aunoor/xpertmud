#include "MCCP.h"

#include <arpa/telnet.h>
extern char* telopts[];

#ifndef TELOPT_COMPRESS
# define TELOPT_COMPRESS 85
#endif
#ifndef TELOPT_COMPRESS2
# define TELOPT_COMPRESS2 86
#endif


MCCP::MCCP(QObject* parent, SenderIface* sender,
	   InputStreamFilter *first, 
	   InputStreamFilter *next):
  QObject(parent), InputStreamFilter(first, next),
  sender(sender), isCompressed(false)
{
}

MCCP::~MCCP() {}

void MCCP::plug(TelnetFilter& telnetFilter) {
  telnetFilter.registerIac(TELOPT_COMPRESS, this);
  telnetFilter.registerIac(TELOPT_COMPRESS2, this);
}
		 
TelnetIacAnswer MCCP::iacWont(int) {
  return TelnetIacAnswer(false);
}
TelnetIacAnswer MCCP::iacWill(int) {
  return TelnetIacAnswer(true, true);
}
TelnetIacAnswer MCCP::iacDont(int) {
  return TelnetIacAnswer(false);
}
TelnetIacAnswer MCCP::iacDo(int) {
  return TelnetIacAnswer(true, true);
}

bool MCCP::iacSubNegotiation(int iac, const string& command) {
  //  qDebug("Hmmmm, got something!");
  bool wasCompressed = isCompressed;
  if((iac == TELOPT_COMPRESS2 && command.size() == 0) || // v2
     (iac == TELOPT_COMPRESS && command.size() == 1 &&  // v1
      command[0] == (char)WILL)) {
    //qDebug("-> start compression");
    isCompressed = true;
  }
  if(wasCompressed != isCompressed) {
    qDebug("compression state changed");
    stream.next_in = Z_NULL;
    stream.avail_in = 0;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    if(inflateInit(&stream) != Z_OK) {
      qWarning("Can't initialize decompression, but the server will compress! Please restart xpertmud!");
    }
    return true;
  }
  return false;
}

#define MCCP_BUFFER_SIZE 10000
void MCCP::parse(const string& str) {
  if(isCompressed) {
    //qDebug(QString("Decompressing a string of length %1").arg(str.length()));
    char inflateBuffer[MCCP_BUFFER_SIZE];
    stream.next_in = (Bytef*)str.data();
    stream.avail_in = str.length();
    int state;
    do {
      //qDebug(QString("Still %1 bytes to decompress...").arg(stream.avail_in));
      stream.next_out = (Bytef*)inflateBuffer;
      stream.avail_out = MCCP_BUFFER_SIZE;
      state = inflate(&stream,  Z_SYNC_FLUSH);
      if(stream.avail_out < MCCP_BUFFER_SIZE) {
	//qDebug(QString("Got %1 uncompressed bytes...")
	//     .arg(MCCP_BUFFER_SIZE-stream.avail_out));
	//qDebug(std::string(inflateBuffer,
	//	   MCCP_BUFFER_SIZE-stream.avail_out).c_str());
	next->parse(std::string((char*)inflateBuffer, 
				MCCP_BUFFER_SIZE-stream.avail_out));
      }
      if(state == Z_STREAM_END) {
	qDebug("Stream ended here...");
	isCompressed = false;
	first->parse(std::string((char*)stream.next_in, stream.avail_in));
	inflateEnd(&stream);
      }
    } while(state == Z_OK && stream.avail_out == 0);
    //const char* errorMsg = zError(state);
    //if(errorMsg != NULL)
    //qDebug(QString("There is an error Message! %1").arg(errorMsg));
    //qDebug(QString("%1 %2 %3 %4 %5").arg(Z_ERRNO).arg(Z_STREAM_ERROR)
    //.arg(Z_DATA_ERROR).arg(Z_MEM_ERROR).arg(Z_BUF_ERROR));
    //qDebug(QString("Sate: %1, Out: %2").arg(state).arg(stream.avail_out));
  } else {
    next->parse(str);
  }
}
