#include "zone.hh"
#include "map.hh"
#include <qcanvas.h>

//debug
#include <qmessagebox.h>

XMMzone::XMMzone(XMMmap *parent, QString *zonename = 0, int zoneid = 0) {
  if (!zonename)
    name = new QString("new zone");
  else
    name = zonename;
  CHECK_PTR(name);
  zones.setAutoDelete(true);
  level = 0;
  next_room_id = 0;
  id = zoneid;
  canvas = new QCanvas(parent, name->latin1());
  CHECK_PTR(canvas);
}

XMMzone::~XMMzone() {
  delete name;
}

XMMzone *XMMzone::findSubZone(int zoneid) {
  return 0;
}

QString XMMzone::getName() {
  return (*name);
}

int XMMzone::getID() {
  return (id);
}
