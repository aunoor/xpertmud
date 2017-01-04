#ifndef KGLOBAL_H
#define KGLOBAL_H

#include "kstandarddirs.h"
#include "kconfig.h"
#include "kaboutdata.h"
#include <QStringList>

#ifndef Q_CHECK_PTR
#define Q_CHECK_PTR(X)
#endif

class KIconLoader;

class KGlobal {
 public:
  static KStandardDirs* dirs() {
    if(_instance == NULL) _instance = new KGlobal();
    return &_instance->d;
  }

  static KConfig* config() {
    if(_instance == NULL) _instance = new KGlobal();
    return &_instance->c;
  }

  static KIconLoader* iconLoader();

  static KGlobal* instance() {
    if(_instance == NULL) _instance = new KGlobal();
    return _instance;
  }

  KAboutData* aboutData() {
    return new KAboutData();
  }

 private:
  static KGlobal *_instance;
  KStandardDirs d;
  KConfig c;
};

#endif
