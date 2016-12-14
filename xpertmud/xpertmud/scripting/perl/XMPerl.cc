#include "XMPerl.h"

#include "PerlInterpret.h"
#include <kglobal.h>
#include <kstandarddirs.h>
#include <cassert>
// TODO: remove
//#include <iostream>
//using std::cout;
//using std::endl;

namespace {
  class RecursionGuard {
  public:
    RecursionGuard(int & rec):recursion(rec) { ++recursion; }
    ~RecursionGuard() { --recursion; }
  private:
    int & recursion;
  };
}

XMPerl::XMPerl(QObject* parent,
	       const char* name):
  XMScripting(parent,name),interp(NULL),recursionCounter(0) {
  
  
}

XMPerl::~XMPerl() {
  if (interp)
    delete interp;
  assert(recursionCounter == 0);
}

void XMPerl::setCallback(GuiScriptingBindings *callBack) {
  wrapper.setCallback(callBack);
  if (!interp) {
    
    interp = new PerlInterpret(&wrapper);

    QStringList incPath=KGlobal::dirs()->findDirs("appdata","perl");
    
    for (QStringList::Iterator it=incPath.begin();
	 it!=incPath.end(); ++it) {
      
      interp->addIncludeDir((*it).latin1());
    }
  }
}

void XMPerl::textEntered(const QString & text) {
  RecursionGuard guard(recursionCounter);
  QCString utf8=text.utf8();
  if (interp)
    interp->textEntered(utf8.data(),utf8.length());
}


bool XMPerl::keyPressed(const QString & key, const QString & ascii) {
  RecursionGuard guard(recursionCounter);
  if (interp)
    return interp->keyPressed(key.latin1(), ascii.latin1());
  return false;
}

void XMPerl::textReceived(const QString & line, int id) {
  RecursionGuard guard(recursionCounter);
  if (interp) {
    QCString utf8 = line.utf8();
    interp->textReceived(utf8.data(), utf8.length(), id);
  }
}

void XMPerl::timer() {
  if (recursionCounter == 0) {
    // Avoid Timer calls in recursion
    RecursionGuard guard(recursionCounter);
    if (interp)
      interp->timer();
  } /* DEBUG */ else {
    printf("IGNORING TIMER while recursion==%d\n",recursionCounter);
  }
}

void XMPerl::echo(bool state, int id) {
  RecursionGuard guard(recursionCounter);
  if (interp)
    interp->echo(state, id);
}

void XMPerl::connectionEstablished(int id) {
  RecursionGuard guard(recursionCounter);
  if(interp)
    interp->connectionEstablished(id);
}

void XMPerl::connectionDropped(int id) {
  RecursionGuard guard(recursionCounter);
  if(interp)
    interp->connectionDropped(id);
}


void XMPerl::mouseDown(int id, int x, int y) {
  RecursionGuard guard(recursionCounter);
  if (interp)
    interp->mouseDown(id,x,y);
}

void XMPerl::mouseDrag(int id, int x, int y) {
  RecursionGuard guard(recursionCounter);
  if (interp)
    interp->mouseDrag(id,x,y);
}

void XMPerl::mouseUp(int id, int x, int y) {
  RecursionGuard guard(recursionCounter);
  if (interp)
    interp->mouseUp(id,x,y);
}


QVariant XMPerl::pluginCall(int id, int function, const QVariant & args) {
  RecursionGuard guard(recursionCounter);
  if (interp) {
      QCString utf8=args.toString().utf8();

#ifdef WIN32
    char * ret=(char*)interp->pluginCall(id,function,utf8.data(),utf8.length());
#else
    const char * ret=interp->pluginCall(id,function,utf8.data(),utf8.length());
#endif
    if (ret) {
	QVariant result( QString::fromUtf8(ret) );

	delete[] ret;

	return result;
    } else {
	return QVariant();
    }
  }
  return 42;
}
