#include "main.hh"
#include "menu.hh"
#include "view.hh"
#include "map.hh"
#include "zonelist.hh"

#include "backend-dummy.hh"
#include "backend-tmw.h"

#include <kgenericfactory.h>

#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

XmudMapper::XmudMapper(QWidget *parent, 
		       const char *name,
		       const QStringList & /* unused: args*/):
  QWidget(parent) {

  m_backend = 0;
  m_map = 0;

  m_mapview = new XMMmapView (this);
  m_mapview->setObjectName("mapperMapView");

  m_zonelist = new XMMzonelist(this);
  m_zonelist->setObjectName("mapperZonelist");

  m_menu = new XMMmenu(this);
  m_menu->setObjectName("mapperMenu");
  m_menu->setNativeMenuBar(false);

  connect(m_menu, SIGNAL(emitSelectBackend(int)),
	        this, SLOT(slotSelectBackend(int)));
  connect(this, SIGNAL(emitBackendSelected(XMMAbstractBackend *)),
          m_menu, SLOT(slotBackendSelected(XMMAbstractBackend *)));

  m_layout = new QVBoxLayout(this);
  m_layout->setMenuBar(m_menu);

  m_splitter = new QSplitter(this);
  m_splitter->setOrientation(Qt::Horizontal);

  m_splitter->addWidget(m_zonelist);
  m_splitter->addWidget(m_mapview);

  m_splitter->setStretchFactor(1, 2);
  m_layout->addWidget(m_splitter);

  setLayout(m_layout);
}

XmudMapper::~XmudMapper() {
  slotCloseMap();
  delete m_backend;
  m_backend = 0;
}

XMMmenu *XmudMapper::getMenuBar() {
  return m_menu;
}

XMMmap *XmudMapper::getMap() {
  return m_map;
}

void XmudMapper::slotFunctionCall(int func, const QVariant & args, QVariant & result) {
  switch(func) {
    case 0: {
      if (m_backend) m_backend->parseLine(args.toString());
      result = QVariant();
    }

  default:
    result="Unknown function called";
  }
}

void XmudMapper::slotSelectBackend(int backend_id) {
  XMMAbstractBackend *newbackend = NULL;

  switch (backend_id) {
    case XMBDummy:
      newbackend = new XMMbackend_dummy(this);
      break;
    case XMBTMW:
      newbackend = new XMMbackend_TMW(this);
      break;
    default:
      break;
  }

  if (newbackend == NULL) return;

  qDebug() << "Backend selected:"<<newbackend->getBackendName();

  if (m_backend) {
    delete m_backend;
    m_backend = 0;
  }

  newbackend->setObjectName("mapperBackend");
  m_backend = newbackend;
  emit emitBackendSelected(m_backend);
  connect(m_backend, SIGNAL(newRoomSignal(XMObject*)), this, SLOT(slotNewRoom(XMObject*)));
}

void XmudMapper::slotNewMap() {
  if (!m_backend) {
    QMessageBox::information(this, "Auto Mapper GUI", "You need to select a backend first");
    return;
  }
  slotCloseMap();
  m_map = new XMMmap(this);
  m_map->setObjectName("mapperMap");

  m_zonelist->setModel(m_map->getModel());

//  mapview->setCanvas(map->canvas());
///  connect(m_mapview, SIGNAL(emitAddRoom(int, int)), m_map, SLOT(slotAddRoom(int, int)));
///  connect(this, SIGNAL(emitAddZone(QString *)), m_map, SLOT(slotAddZone(QString *)));
  connect(m_map, SIGNAL(emitZoneAdded(QString, int)), m_zonelist, SLOT(slotAddZone(QString, int)));
  connect(m_map, SIGNAL(emitChangeZone(int)), this, SLOT(slotChangeZone(int)));
  emit emitAddZone(0);
}

void XmudMapper::slotOpenMap() {
  if (!m_backend) {
    QMessageBox::information(this, "Auto Mapper GUI", "You need to select a backend first");
    return;
  }
  slotCloseMap();
}

void XmudMapper::slotCloseMap() {
  slotSaveMap();
//  mapview->setCanvas(0);
}

void XmudMapper::slotSaveMap() {
}

void XmudMapper::slotNewZoneDialog() {
  bool ok = false;
  QString zonename = QInputDialog::getText(this, "New Zone Name", "Please enter the new zone's name", QLineEdit::Normal, QString::null, &ok);
  if (ok) {
    emit emitAddZone(zonename);
  }
}

void XmudMapper::slotDelZoneDialog() {
  int btn = QMessageBox::warning(this, "Delete Zone", "You're about to delete the current zone", "OK", "Cancel", QString::null, 1, 1);
  if (btn == 0)
    emit emitDelZone();
}

void XmudMapper::slotRenameZoneDialog() {
  bool ok = false;
  QString zonename = QInputDialog::getText(this, "Zone New Name", "Please enter the current zone's new name", QLineEdit::Normal, QString::null, &ok);
  if (ok) {
    emit emitRenameZone(zonename);
  }
}

void XmudMapper::slotChangeZone(int zoneid) {
//  mapview->setCanvas(map->canvas());
  // XXX changer l'item courant dans zonelist
}

void XmudMapper::slotAddTrigger() {
  QVariant result;
  emit callback(0, QVariant(), result);
}

void XmudMapper::slotDelTrigger() {
  QVariant result;
  emit callback(1, QVariant(), result);
}

void XmudMapper::slotSendLine(QString line) {
  QVariant result;
  emit callback(2, line, result);
}

void XmudMapper::slotNewRoom(XMObject *object) {
  if (m_map==NULL) return;

  m_map->addNewRoom(object);
}

K_EXPORT_COMPONENT_FACTORY( xmud_mapper, KGenericFactory<XmudMapper>( "xmud_mapper" ) );
