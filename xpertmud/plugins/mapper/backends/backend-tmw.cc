#include "backend-tmw.h"
#include "main.hh"
#include "menu.hh"

#include <QDebug>

XMMbackend_TMW::XMMbackend_TMW(QObject *parent) : XMMAbstractBackend(parent) {
  menu = new QMenu("XmTmwMenu");
  menu->setTitle("Operations");

  menu->addAction("Autogenerate map", this, SLOT(slotAutoGenerateMap()));

  XmudMapper * mudMapper = getParent();

  if (mudMapper==NULL) return;

  mudMapper->getMenuBar()->addMenu(menu);

}

XMMbackend_TMW::~XMMbackend_TMW() {
  delete menu;
  if(getParent()) {
    getParent()->slotDelTrigger();
  }
}

QString XMMbackend_TMW::getBackendName() {
  return QStringLiteral("TinyMU* backend");
}

void XMMbackend_TMW::slotAutoGenerateMap() {
  if(getParent()) {
    getParent()->slotAddTrigger();
  }
}

XMBackends XMMbackend_TMW::getId() {
  return XMBTMW;
}

void XMMbackend_TMW::parseLine(QString line) {
  qDebug() << "line:" << line;
}
