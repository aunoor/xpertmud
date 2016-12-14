// -*- c++ -*-
#ifndef IACHANDLERCOLLECTION_H
#define IACHANDLERCOLLECTION_H

#include <qobject.h>
#include <qwidget.h>
#include "TelnetFilter.h"

#include <vector>
#include <string>

class IacHandlerCollection: public QObject, public TelnetIacCallback {
  Q_OBJECT
public:
  IacHandlerCollection(QObject *parent, SenderIface *sender);
  virtual ~IacHandlerCollection();

  void plug(TelnetFilter& telnetFilter);
		 
  TelnetIacAnswer iacWont(int iac);
  TelnetIacAnswer iacWill(int iac);
  TelnetIacAnswer iacDont(int iac);
  TelnetIacAnswer iacDo(int iac);
  bool iacSubNegotiation(int iac, const std::string& command);

  /**
   * Register a terminal type name. Each time
   * the server requests a ttype, the next name
   * from the list is sent.
   */
  void registerTType(const std::string& name);

signals:
  void echoChanged(bool);

private:
  SenderIface *sender;
  std::vector<std::string> ttypes;
  unsigned int ttypeRequestNr;
};

#endif
