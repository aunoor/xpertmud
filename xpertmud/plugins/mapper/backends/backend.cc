#include "main.hh"
#include "backend.hh"
#include <QMessageBox>

XMMAbstractBackend::XMMAbstractBackend(QObject *parent) :
  QObject(parent) {
  m_parent = dynamic_cast<XmudMapper*>(parent);
}

XMMAbstractBackend::~XMMAbstractBackend() {
}

void XMMAbstractBackend::configShow(QWidget *parent) {
  QMessageBox::information(parent, getBackendName(), "No configuration is required for this backend");
}

void XMMAbstractBackend::slotConfig(QWidget *parent) {
  configShow(parent);
}

XmudMapper *XMMAbstractBackend::getParent() {
  return m_parent;
}

void XMMAbstractBackend::parseLine(QString line) {

}
