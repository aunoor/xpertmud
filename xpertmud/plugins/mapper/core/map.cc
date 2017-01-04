#include "map.hh"
#include <qcanvas.h>
#include <qstring.h>

//debug
#include <qmessagebox.h>

XMMmap::XMMmap(QObject *parent = 0, 
		     const char *name = 0,
		     const char *mapname = 0):
  QObject(parent, name) {
  zone = 0;
  next_zone_id = 0;
  zones.setAutoDelete(true);
}

XMMmap::~XMMmap() {
}

QCanvas *XMMmap::canvas() {
  if (zone)
    return zone->canvas;
  else
    return 0;
}

XMMzone *XMMmap::findZone(int zoneid) {
  XMMzone *tmpzone = 0;
  XMMzone *subzone = 0;
  tmpzone = zones.first();
  while (tmpzone) {
    if (tmpzone->getID() == zoneid)
      return tmpzone;
    if ((subzone = tmpzone->findSubZone(zoneid)))
      return subzone;
    tmpzone = zones.next();
  }
  return 0;
}

void XMMmap::slotSelectZone(int zoneid) {
  XMMzone *tmpzone;

  CHECK_PTR(zone);
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

void XMMmap::slotAddZone(QString *zonename) {
  XMMzone *newzone = new XMMzone(this, zonename, next_zone_id);
  CHECK_PTR(newzone);
  next_zone_id++;
  zones.append(newzone);
  emit emitZoneAdded(newzone->getName(), newzone->getID());
  if (!zone) {
    zone = newzone;
    emit emitChangeZone(newzone->getID());
  }
}
