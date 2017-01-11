#include "backend-dummy.hh"


XMMbackend_dummy::XMMbackend_dummy(QObject *parent):
  XMMAbstractBackend(parent) {
}

XMMbackend_dummy::~XMMbackend_dummy(){
}

QString XMMbackend_dummy::getBackendName() {
  return QStringLiteral("Dummy backend");
}

XMBackends XMMbackend_dummy::getId() {
  return XMBDummy;
}
