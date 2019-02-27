#include "IacHandlerCollection.h"

#include <arpa/telnet.h>
extern char* telopts[];

#include <iostream>
using std::cout;
using std::endl;

IacHandlerCollection::IacHandlerCollection(QObject *parent,
					   SenderIface *sender):
  QObject(parent), sender(sender), ttypeRequestNr(0) {}

IacHandlerCollection::~IacHandlerCollection() {}

void IacHandlerCollection::plug(TelnetFilter& telnetFilter) {
  telnetFilter.registerIac(TELOPT_ECHO, this);
  telnetFilter.registerIac(TELOPT_SGA, this);
  telnetFilter.registerIac(TELOPT_LFLOW, this);
  telnetFilter.registerIac(TELOPT_TTYPE, this);
  telnetFilter.registerIac(TELOPT_TSPEED, this);
  telnetFilter.registerIac(TELOPT_NAWS, this);
  telnetFilter.registerIac(TELOPT_XDISPLOC, this);
  telnetFilter.registerIac(TELOPT_NEW_ENVIRON, this);
  telnetFilter.registerIac(TELOPT_STATUS, this);
}

TelnetIacAnswer IacHandlerCollection::iacWont(int iac) {
  switch(iac) {
  case TELOPT_ECHO:
    cout << "WONT ECHO" << endl;
    emit echoChanged(true);
    return TelnetIacAnswer(true, false);
    break;
  case TELOPT_SGA:
    return TelnetIacAnswer(true, true);
    break;
  case TELOPT_LFLOW:
  case TELOPT_TTYPE:
  case TELOPT_TSPEED:
  case TELOPT_NAWS:
  case TELOPT_XDISPLOC:
  case TELOPT_NEW_ENVIRON:
  case TELOPT_STATUS:
    return TelnetIacAnswer(false);
    break;
  }
  return TelnetIacAnswer(false);
}

TelnetIacAnswer IacHandlerCollection::iacWill(int iac) {
  switch(iac) {
  case TELOPT_ECHO:
    cout << "WILL ECHO" << endl;
    emit echoChanged(false); 
    break;
  case TELOPT_TTYPE:
    if(ttypes.size() > 0)
      return TelnetIacAnswer(true, true);
    else
      return TelnetIacAnswer(true, false);
    break;
  case TELOPT_SGA:
    return TelnetIacAnswer(true, true);
    break;
  case TELOPT_LFLOW:
  case TELOPT_TSPEED:
  case TELOPT_NAWS:
  case TELOPT_XDISPLOC:
  case TELOPT_NEW_ENVIRON:
  case TELOPT_STATUS:
    return TelnetIacAnswer(true, false);
    break;
  }
  return TelnetIacAnswer(false);
}

TelnetIacAnswer IacHandlerCollection::iacDont(int iac) {
  switch(iac) {
  case TELOPT_ECHO:
    cout << "DONT ECHO" << endl;
    emit echoChanged(false);
    return TelnetIacAnswer(true, false);
    break;
  case TELOPT_SGA:
    return TelnetIacAnswer(true, true);
    break;
  case TELOPT_LFLOW:
  case TELOPT_TTYPE:
  case TELOPT_TSPEED:
  case TELOPT_NAWS:
  case TELOPT_XDISPLOC:
  case TELOPT_NEW_ENVIRON:
  case TELOPT_STATUS:
    return TelnetIacAnswer(false);
    break;
  }
  return TelnetIacAnswer(false);
}

TelnetIacAnswer IacHandlerCollection::iacDo(int iac) {
  switch(iac) {
  case TELOPT_ECHO:
    cout << "DO ECHO" << endl;
    emit echoChanged(true);
    break;
  case TELOPT_TTYPE:
    if(ttypes.size() > 0)
      return TelnetIacAnswer(true, true);
    else
      return TelnetIacAnswer(true, false);
    break;
  case TELOPT_SGA:
    return TelnetIacAnswer(true, true);
    break;
  case TELOPT_LFLOW:
  case TELOPT_TSPEED:
  case TELOPT_NAWS:
  case TELOPT_XDISPLOC:
  case TELOPT_NEW_ENVIRON:
  case TELOPT_STATUS:
    return TelnetIacAnswer(true, false);
    break;
  }
  return TelnetIacAnswer(false);
}

bool IacHandlerCollection::iacSubNegotiation(int iac, const string& command) {
  switch(iac) {
  case TELOPT_TTYPE:
    if(command.length() == 1 && command[0] == TELQUAL_SEND) {
      if(ttypes.size() > ttypeRequestNr) {
	sender->sendByte((char)IAC);
	char cmd[] = 
	  {(char)SB,(char)TELOPT_TTYPE,
	   (char)TELQUAL_IS};
	sender->send(cmd, 3);
	sender->send(ttypes[ttypeRequestNr]);
	sender->sendByte((char)IAC);
	sender->sendByte((char)SE);
	if(ttypes.size() > ttypeRequestNr+1)
	  ++ttypeRequestNr;
      }
    } else {
      cout << "evil error" << endl;
    }
    break;
  default:
    cout << "Subnegotiation started for " << telopts[iac] << endl
	 << "and we're not prepared for that..." << endl;
  }
  return false;
}

void  IacHandlerCollection::registerTType(const std::string& name) {
  ttypes.push_back(name);
}
