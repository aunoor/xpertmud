#include "map.hh"

#include <QMessageBox>

XMMmap::XMMmap(QObject *parent, QString mapname):
  QObject(parent) {
  zone = 0;
  next_zone_id = 0;
}

XMMmap::~XMMmap() {
  qDeleteAll(zones);
}

QCanvas *XMMmap::canvas() {
  if (zone)
    return zone->canvas;
  else
    return 0;
}

XMMzone *XMMmap::findZone(int zoneid) {
  foreach(XMMzone *tmpZone, zones) {
      if (tmpZone->getID() == zoneid) return tmpZone;
      XMMzone *subZone = tmpZone->findSubZone(zoneid);
      if (subZone != NULL) return subZone;
  }
  return NULL;
}

void XMMmap::slotSelectZone(int zoneid) {
  XMMzone *tmpzone;

  if (zoneid != zone->getID()) {
    tmpzone = findZone(zoneid);
    if (tmpzone) {
      zone = tmpzone;
      emit emitChangeZone(zoneid);
    }
  }
}

void XMMmap::slotAddRoom(int x, int y) {
  QString msg = "Received order to add Room at x = %1, y = %2";
  QMessageBox::information(0, "mapperMap debug", msg.arg(x).arg(y));
}

void XMMmap::slotAddZone(QString zonename) {
  XMMzone *newzone = new XMMzone(this, zonename, next_zone_id);
  next_zone_id++;
  zones.append(newzone);
  emit emitZoneAdded(newzone->getName(), newzone->getID());
  if (!zone) {
    zone = newzone;
    emit emitChangeZone(newzone->getID());
  }
}
