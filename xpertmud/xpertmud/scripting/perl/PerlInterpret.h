// -*- c++ -*-
#ifndef PERLINTERPRET_H
#define PERLINTERPRET_H

#ifdef WIN32
#define _M_M68K
#endif

typedef struct interpreter PerlInterpreter;

class GuiScriptingWrapper;

class PerlInterpret {

public:
  PerlInterpret(GuiScriptingWrapper *callBack);
  ~PerlInterpret();

  void setCallback(GuiScriptingWrapper *callBack);

  void addIncludeDir(const char * const inc);

  void timer();
  void textEntered(const char * const text, int length);

  bool keyPressed(const char * const key, const char * const ascii);

  void textReceived(const char * const line, int length, int id);
  void echo(bool state, int id);
  void connectionEstablished(int id);
  void connectionDropped(int id);

  void mouseDown(int id, int x, int y);
  void mouseDrag(int id, int x, int y);
  void mouseUp(int id, int x, int y);
  const char * pluginCall(int id, int function, const char * const args, int length);

private:
  PerlInterpreter *perlInt;
  GuiScriptingWrapper *callBack;
};

#endif
