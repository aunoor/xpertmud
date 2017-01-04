#ifndef KGENERICFACTORY_H
#define KGENERICFACTORY_H

#include "klibloader.h" 

template<class T>
class KGenericFactory: public KLibFactory {
 public:
  KGenericFactory(const char *name = 0):
    KLibFactory(0, name) {
  }

  virtual QObject *createObject(QObject *parent,
			const char *name,
			const char *classname,
			const QStringList &args) {
    return new T((QWidget *)parent, name, args);
  }
};

#endif
