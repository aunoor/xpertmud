// -*- c++ -*-
#ifndef DCOP_XPERTMUD_H
#define DCOP_XPERTMUD_H

#include <dcopobject.h>


class XpertmudIface: virtual public DCOPObject {
  K_DCOP
k_dcop:
  virtual void doConnect(const QString & host, /* unsigned short */int port)=0;
  virtual void doDisconnect()=0;
  virtual void send(const QString & text)=0;

};

#endif
