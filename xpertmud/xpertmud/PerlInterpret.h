// -*- c++ -*-
#ifndef PERLINTERPRET_H
#define PERLINTERPRET_H

typedef struct interpreter PerlInterpreter;

class GuiScriptingBindings;

class PerlInterpret {
public:
  PerlInterpret(GuiScriptingBindings *callBack);
  ~PerlInterpret();


  void addIncludeDir(const char * const inc);
  void textEntered(const char * const text);
  bool keyPressed(const char * const key, const char * const ascii);
  void textReceived(const char * const line);
  void timer();
  void echo(bool state);

private:
  //PerlInterpreter *perlInt;
  GuiScriptingBindings *callBack;
  static int iNumber;
};

#endif
