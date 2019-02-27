// -*- c++ -*-
#ifndef DCOP_SCRIPTING_H
#define DCOP_SCRIPTING_H

#include <dcopobject.h>

class ScriptingIface: virtual public DCOPObject {
  K_DCOP
k_dcop:
  virtual void textEntered(const QString & text) = 0;
  virtual bool keyPressed(const QString & key, const QString & ascii) = 0;
  virtual void textReceived(const QString & line, int id) = 0;
  virtual void echo(bool state, int id) = 0;

  // TODO: Those are better implemented in the TextWindow Iface?
  //  virtual void mouseDown(int id, int x, int y) = 0;
  //  virtual void mouseDrag(int id, int x, int y) = 0;
  //  virtual void mouseUp(int id, int x, int y) = 0;

};

#endif
