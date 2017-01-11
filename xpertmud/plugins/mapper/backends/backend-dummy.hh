#ifndef XMUD_BACKENDDUMMY_H
#define XMUD_BACKENDDUMMY_H
#include "backend.hh"

class QWidget;

class XMMbackend_dummy:public XMMAbstractBackend {
  Q_OBJECT
public:
  XMMbackend_dummy(QObject *parent = 0);
  ~XMMbackend_dummy();

  QString getBackendName() override;

  XMBackends getId() override;
};

#endif
