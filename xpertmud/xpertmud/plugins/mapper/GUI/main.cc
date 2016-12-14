#include "main.hh"
#include "menu.hh"
#include "view.hh"
#include "backend-dummy.hh"
#include "map.hh"
#include "zonelist.hh"
#include <kgenericfactory.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qcanvas.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qlineedit.h>

XmudMapper::XmudMapper(QWidget *parent, 
		       const char *name,
		       const QStringList & /* unused: args*/):
  QWidget(parent,name) {

  backend = 0;
  map = 0;

  mapview = new XMMmapView (this, "mapperMapView");
  CHECK_PTR(mapview);

  zonelist = new XMMzonelist(this, "mapperZonelist");
  CHECK_PTR(zonelist);

  menu = new XMMmenu(this, "mapperMenu");
  CHECK_PTR(menu);
  connect(menu, SIGNAL(emitSelectBackend(int)), 
	  this, SLOT(slotSelectBackend(int)));
  connect(this, SIGNAL(emitBackendSelected(XMMbackend *)), 
	  menu, SLOT(slotBackendSelected(XMMbackend *)));

  layout = new QHBoxLayout(this, 2);
  CHECK_PTR(layout);
  layout->addWidget(zonelist);
  layout->addWidget(mapview);
}

XmudMapper::~XmudMapper() {
  slotCloseMap();
  delete backend;
  backend = 0;
  delete menu;
  menu = 0;
  delete mapview;
  mapview = 0;
  delete layout;
  layout = 0;
}

void XmudMapper::slotFunctionCall(int func, const QVariant & args, QVariant & result) {
  switch(func) {
  default:
    result="Unknown function called";
  }
}

void XmudMapper::slotSelectBackend(int backend_id) {
  XMMbackend *newbackend = 0;
  if (backend) {
    delete backend;
    backend = 0;
  }
  if (backend_id == BACKEND_DUMMY) {
    newbackend = new XMMbackend_dummy(this, "mapperBackend");
  }
  CHECK_PTR(newbackend);
  backend = newbackend;
  emit emitBackendSelected(backend);
}

void XmudMapper::slotNewMap() {
  if (!backend) {
    QMessageBox::information(this, "Auto Mapper GUI", "You need to select a backend first");
    return;
  }
  slotCloseMap();
  map = new XMMmap(this, "mapperMap", 0);
  CHECK_PTR(map);
  mapview->setCanvas(map->canvas());
  connect(mapview, SIGNAL(emitAddRoom(int, int)), map, SLOT(slotAddRoom(int, int)));
  connect(this, SIGNAL(emitAddZone(QString *)), map, SLOT(slotAddZone(QString *)));
  connect(map, SIGNAL(emitZoneAdded(QString, int)), zonelist, SLOT(slotAddZone(QString, int)));
  connect(map, SIGNAL(emitChangeZone(int)), this, SLOT(slotChangeZone(int)));
  emit emitAddZone(0);
}

void XmudMapper::slotOpenMap() {
  if (!backend) {
    QMessageBox::information(this, "Auto Mapper GUI", "You need to select a backend first");
    return;
  }
  slotCloseMap();
}

void XmudMapper::slotCloseMap() {
  slotSaveMap();
  mapview->setCanvas(0);
}

void XmudMapper::slotSaveMap() {
}

void XmudMapper::slotNewZoneDialog() {
  bool ok = false;
  QString zonename = QInputDialog::getText("New Zone Name", "Please enter the new zone's name", QLineEdit::Normal, QString::null, &ok, this, 0);
  if (ok) {
    emit emitAddZone(&zonename);
  }
}

void XmudMapper::slotDelZoneDialog() {
  int btn = QMessageBox::warning(this, "Delete Zone", "You're about to delete the current zone", "OK", "Cancel", QString::null, 1, 1);
  if (btn == 0)
    emit emitDelZone();
}

void XmudMapper::slotRenameZoneDialog() {
  bool ok = false;
  QString zonename = QInputDialog::getText("Zone New Name", "Please enter the current zone's new name", QLineEdit::Normal, QString::null, &ok, this, 0);
  if (ok) {
    emit emitRenameZone(&zonename);
  }
}

void XmudMapper::slotChangeZone(int zoneid) {
  mapview->setCanvas(map->canvas());
  // XXX changer l'item courant dans zonelist
}

K_EXPORT_COMPONENT_FACTORY( xmud_mapper, KGenericFactory<XmudMapper>( "xmud_mapper" ) );
