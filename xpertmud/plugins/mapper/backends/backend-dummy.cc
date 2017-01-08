#include "backend-dummy.hh"
#include <qwidget.h>
#include <qmessagebox.h>

XMMbackend_dummy::XMMbackend_dummy(QObject *parent):
  XMMbackend(parent) {
}

XMMbackend_dummy::~XMMbackend_dummy(){
}

void XMMbackend_dummy::slotConfig(QWidget *parent = 0) {
  QMessageBox::information(parent, "Dummy backend", "No configuration is required for this backend");
}
