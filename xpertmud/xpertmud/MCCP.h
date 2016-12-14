#ifndef MCCP_H
#define MCCP_H

#include <qobject.h>
#include <zlib.h>
#include <vector>
#include "TelnetFilter.h"
#include "InputStreamHandler.h"

class MCCP: public QObject,  public TelnetIacCallback, public InputStreamFilter {
  Q_OBJECT
public:
  MCCP(QObject* parent, SenderIface* sender,
       InputStreamFilter *first = NULL, 
       InputStreamFilter *next = NULL);
  virtual ~MCCP();

  void  plug(TelnetFilter& telnetFilter);
		 
  TelnetIacAnswer iacWont(int iac);
  TelnetIacAnswer iacWill(int iac);
  TelnetIacAnswer iacDont(int iac);
  TelnetIacAnswer iacDo(int iac);
  bool iacSubNegotiation(int iac, const string& command);

  virtual void parse(const string& str);

private:
  SenderIface* sender;

  bool isCompressed;
  z_stream stream;
};

#endif
