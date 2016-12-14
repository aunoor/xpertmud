#ifndef KLIBLOADER_H
#define KLIBLOADER_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include "kstandarddirs.h"
#include "kglobal.h"

#include <iostream>
using std::cout;
using std::endl;



#ifdef WIN32
#define K_EXPORT_COMPONENT_FACTORY(X, Y) \
extern "C" { __declspec(dllexport) void *init_##X() { return new Y; } }
#else
#define K_EXPORT_COMPONENT_FACTORY(X, Y) \
extern "C" { void *init_##X() { return new Y; } }
#endif

class KLibFactory: public QObject {
  Q_OBJECT
 public:
  KLibFactory() {}
  KLibFactory(QObject *parent, const char* name) {
  }
  virtual ~KLibFactory() {}

  virtual QObject* create(QObject* parent, const QString& text1,
		  const QString& text2) {
    return createObject(parent, text1, text2, "");
  }

  virtual QObject* createObject(QObject* parent, const char* name,
				const char* classname, const QStringList& args)  = 0;

};

class KLibrary {
 public:
  KLibrary(KLibFactory *f): f(f) {}
  KLibFactory *factory() { return f; }
 private:
  KLibFactory *f;
};

class KLibLoader {
 public:
  void unloadLibrary(const QString& name) {
  }

  static KLibLoader* self() { 
    if(instance == NULL) {
      instance = new KLibLoader();
    }
    return instance; 
  }

  KLibrary* globalLibrary(const QString& libname) {
    return new KLibrary(factory(libname));
  }

  KLibFactory* factory(const QString& libname) {
    cout << "trying to load lib " << libname.latin1() << endl;
#ifdef WIN32
    QString name = libname + ".dll";
#else
# ifdef MACOS_X
    QString name = libname + ".dylib";
# else 
    QString name = libname + ".so";
# endif
#endif
    QStringList list = 
      KGlobal::dirs()->findAllResources("module", name, false, true);
    if(list.count() == 0) { 
      cout << "Couldn't find library " << name.latin1() << endl;
      return 0; 
    }
    cout << list.first().latin1() << endl;
    
    QString function = QString("init_") + libname;
    cout << "QLibrary::resolve('" << list.first().latin1() << "', '" << function.latin1() << "');" << endl;
    void *symAddr = QLibrary::resolve(list.first().latin1(), function.latin1());

    if(symAddr == NULL) { cout << "buuuuh2" << endl; return 0; }
    
    KLibFactory* fac = ((KLibFactory* (*)())symAddr)();
    
    cout << "got a fac..." << endl;
    return fac;
  }

  QString lastErrorMessage() { return ""; }
 private:
  KLibLoader() {}
  static KLibLoader* instance;
};


#endif
