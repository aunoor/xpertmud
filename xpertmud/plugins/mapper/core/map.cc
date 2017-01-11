#include "map.hh"

#include <QMessageBox>

XMMmap::XMMmap(QObject *parent, QString mapname):
  QObject(parent) {
  //zone = 0;
  m_zoneModel = new XMZoneModel(this);
}

XMMmap::~XMMmap() {
  //qDeleteAll(zones);
}

XMZoneModel *XMMmap::getModel() {
  return m_zoneModel;
}

QCanvas *XMMmap::canvas() {
/*
  if (zone)
    return zone->canvas;
  else
    return 0;
*/
}



void XMMmap::slotSelectZone(int zoneid) {
/*
  XMObject *tmpzone;

  if (zoneid != zone->getID()) {
    tmpzone = findZone(zoneid);
    if (tmpzone) {
      zone = tmpzone;
      emit emitChangeZone(zoneid);
    }
  }
*/
}

void XMMmap::slotAddRoom(int x, int y) {
  QString msg = "Received order to add Room at x = %1, y = %2";
  QMessageBox::information(0, "mapperMap debug", msg.arg(x).arg(y));
}

void XMMmap::slotAddZone(QString zonename) {
/*
  XMObject *newzone = new XMObject(this, zonename, next_zone_id);
  next_zone_id++;
  zones.append(newzone);
  emit emitZoneAdded(newzone->getName(), newzone->getID());
  if (!zone) {
    zone = newzone;
    emit emitChangeZone(newzone->getID());
  }
*/
}
