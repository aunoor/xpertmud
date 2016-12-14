#include "menu.hh"
#include "backend.hh"
#include <qpopupmenu.h>

XMMmenu::XMMmenu(QWidget *parent = 0, 
		       const char *name = 0):
  QMenuBar(parent, name) {
  files = new QPopupMenu(this, "mapperFilesMenu");
  CHECK_PTR(files);
  files->insertItem("&New Map", parent, SLOT(slotNewMap()), 0, 0, 0);
  files->insertItem("&Open Map", parent, SLOT(slotOpenMap()), 0, 1, 1);
  files->insertItem("&Save Map", parent, SLOT(slotSaveMap()), 0, 2, 2);
  files->insertItem("&Close Map", parent, SLOT(slotCloseMap()), 0, 3, 3);
  
  settings = new QPopupMenu(this, "mapperSettingsMenu");
  CHECK_PTR(settings);
  settings->insertItem("Select &Backend", this, SLOT(slotSelectBackend()), 0, 0, 0);
  settings->insertItem("Configure Backend", this, SLOT(slotConfigBackend()), 0, 1, 1);
  settings->setItemEnabled(1, false);

  zones = new QPopupMenu(this, "mapperZonesMenu");
  CHECK_PTR(zones);
  zones->insertItem("&New Zone", parent, SLOT(slotNewZoneDialog()), 0, 0, 0);
  zones->insertItem("&Delete current zone", parent, SLOT(slotDelZoneDialog()), 0, 1, 1);
  zones->insertItem("&Rename current zone", parent, SLOT(slotRenameZoneDialog()), 0, 2, 2);

  edit = new QPopupMenu(this, "mapperEditMenu");
  CHECK_PTR(edit);
  edit->insertItem("&Zones", zones, 0, 0);

  this->insertItem("&Files", files);
  this->insertItem("&Edit", edit);
  this->insertItem("&Settings", settings);
}


XMMmenu::~XMMmenu() {
  delete settings;
  settings = 0;
  delete files;
  files = 0;
}

void XMMmenu::slotConfigBackend() {
  emit emitConfigBackend(this);
}

void XMMmenu::slotSelectBackend() {
  emit emitSelectBackend(BACKEND_DUMMY);
}

void XMMmenu::slotBackendSelected(XMMbackend *backend) {
  connect(this, SIGNAL(emitConfigBackend(QWidget *)), backend, SLOT(slotConfig(QWidget *)));
  settings->setItemEnabled(1, true);
}
