#ifndef KLIBLOADER_H
#define KLIBLOADER_H

#include <QString>
#include <QStringList>
#include <QLibrary>
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
  KLibFactory(QObject *parent, const char* name): QObject(parent) {
    setObjectName(name);
  }
  virtual ~KLibFactory() {}

  virtual QObject* create(QObject* parent, const QString& text1,
		  const QString& text2) {
    return createObject(parent, text1.toLatin1().data(), text2.toLatin1().data(), QStringList(""));
  }

  virtual QObject* createObject(QObject* parent, const char* name,
				const char* classname, const QStringList& args)  = 0;

};

class KLibrary {
 public:
  KLibrary(KLibFactory *f): f(f) {
    if (f!=NULL) name = f->objectName();
  }
  KLibFactory *factory() { return f; }
  QString libName() {return name;}
  bool unload() {
    QLibrary lib(libName());
    delete f;
    f=NULL;
    return lib.unload();
  }
 private:
  KLibFactory *f;
  QString name;
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
    cout << "trying to load lib " << libname.toLocal8Bit().data() << endl;
#ifdef WIN32
    QString name = libname + ".dll";
#else
# ifdef Q_OS_MACOS
    QString name = libname + ".dylib";
# else 
    QString name = libname + ".so";
# endif
#endif
    QStringList list = 
      KGlobal::dirs()->findAllResources("module", name, false, true);
    if(list.count() == 0) { 
      cout << "Couldn't find library " << name.toLocal8Bit().data() << endl;
      return 0; 
    }
    //cout << list.first().toLocal8Bit().data() << endl;
    
    QString function = QString("init_") + libname;
    cout << "QLibrary::resolve('" << list.first().toLocal8Bit().data() << "', '" << function.toLatin1().data() << "');" << endl;
    QFunctionPointer symAddr = QLibrary::resolve(list.first(), function.toLatin1().data());
    //void *symAddr = QLibrary::resolve(list.first(), function.toLocal8Bit().data());

    if(symAddr == NULL) { cout << "Can't load library or resolve func" << endl; return 0; }
    
    KLibFactory* fac = ((KLibFactory* (*)())symAddr)();
    fac->setObjectName(list.first());
    
    cout << "got a factory..." << endl;
    return fac;
  }

  QString lastErrorMessage() {
    //TODO: add support for error messages
    return "";
  }
 private:
  KLibLoader() {}
  static KLibLoader* instance;
};


#endif
