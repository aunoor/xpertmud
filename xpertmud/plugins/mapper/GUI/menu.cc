#include "menu.hh"
#include "backend.hh"

#include <QMenu>
#include <QDebug>

XMMmenu::XMMmenu(QWidget *parent):
  QMenuBar(parent) {
  QAction *ta;

  files = new QMenu("mapperFilesMenu", this);
  files->setTitle("&Files");

  files->addAction("&New Map", parent, SLOT(slotNewMap()));
  files->addAction("&Open Map", parent, SLOT(slotOpenMap()));
  files->addAction("&Save Map", parent, SLOT(slotSaveMap()));
  files->addAction("&Close Map", parent, SLOT(slotCloseMap()));

  backends = new QMenu("backendListMenu", this);
  backends->setTitle("Select Backend");
  ta = backends->addAction("None", this, SLOT(slotSelectBackend()));
  ta->setProperty("id", (int)XMBDummy);
  ta->setCheckable(true);
  ta = backends->addAction("TinyMU*  Wizard", this, SLOT(slotSelectBackend()));
  ta->setProperty("id", (int)XMBTMW);
  ta->setCheckable(true);

  settings = new QMenu("mapperSettingsMenu", this);
  settings->setTitle("&Settings");
  settings->addMenu(backends);

  zones = new QMenu("mapperZonesMenu", this);
  zones->setTitle("&Zones");
  zones->addAction("&New Zone", parent, SLOT(slotNewZoneDialog()));
  zones->addAction("&Delete current zone", parent, SLOT(slotDelZoneDialog()));
  zones->addAction("&Rename current zone", parent, SLOT(slotRenameZoneDialog()));

  edit = new QMenu("mapperEditMenu", this);
  edit->setTitle("&Edit");
  edit->addMenu(zones);

  this->addMenu(files);
  this->addMenu(edit);
  this->addMenu(settings);
}


XMMmenu::~XMMmenu() {
  delete settings;
  settings = 0;
  delete files;
  files = 0;
}

void XMMmenu::slotSelectBackend() {
  QAction *ta = dynamic_cast<QAction*>(sender());
  if (ta==NULL) return;
  int id;
  bool ok;
  id = ta->property("id").toInt(&ok);
  if (!ok) return;

  emit emitSelectBackend(id);
}

void XMMmenu::slotBackendSelected(XMMAbstractBackend *backend) {
  qDebug() << "backend id="<<backend->getId();
  foreach(QAction *ta, backends->actions()) {
      ta->setChecked(ta->property("id").toInt()==backend->getId());
  }
}
