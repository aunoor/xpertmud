#ifndef XMUD_BACKENDDUMMY_H
#define XMUD_BACKENDDUMMY_H
#include "backend.hh"
#include <qobject.h>

class QWidget;


class XMMbackend_dummy:public XMMbackend {
  Q_OBJECT
public:
  XMMbackend_dummy(QObject *parent, const char *name);
  ~XMMbackend_dummy();

public slots:
  void slotConfig(QWidget *parent);
}; 

#endif
