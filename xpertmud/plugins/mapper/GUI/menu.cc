#include "menu.hh"
#include "backend.hh"
#include <QMenu>

XMMmenu::XMMmenu(QWidget *parent):
  QMenuBar(parent) {
  QAction *ta;

  files = new QMenu("mapperFilesMenu", this);
  files->setTitle("&Files");

  files->addAction("&New Map", parent, SLOT(slotNewMap()));
  files->addAction("&Open Map", parent, SLOT(slotOpenMap()));
  files->addAction("&Save Map", parent, SLOT(slotSaveMap()));
  files->addAction("&Close Map", parent, SLOT(slotCloseMap()));
  
  settings = new QMenu("mapperSettingsMenu", this);
  settings->setTitle("&Settings");
  settings->addAction("Select &Backend", this, SLOT(slotSelectBackend()));
  ta=settings->addAction("Configure Backend", this, SLOT(slotConfigBackend()));
  ta->setProperty("id",1);
  ta->setEnabled(false);

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

void XMMmenu::slotConfigBackend() {
  emit emitConfigBackend(this);
}

void XMMmenu::slotSelectBackend() {
  emit emitSelectBackend(BACKEND_DUMMY);
}

void XMMmenu::slotBackendSelected(XMMbackend *backend) {
  connect(this, SIGNAL(emitConfigBackend(QWidget *)), backend, SLOT(slotConfig(QWidget *)));
  foreach(QAction *ta, settings->actions()) {
      if (ta->property("id").toInt()==1) {
        ta->setEnabled(true);
        break;
      }
  }
}
