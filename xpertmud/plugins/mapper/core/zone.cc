#include "zone.hh"
#include "map.hh"

XMMzone::XMMzone(XMMmap *parent, QString zonename, int zoneid) {
  if (zonename.isEmpty())
    name = QString("new zone");
  else
    name = zonename;
  level = 0;
  next_room_id = 0;
  id = zoneid;
//  canvas = new QCanvas(parent, name->latin1());
}

XMMzone::~XMMzone() {

}

XMMzone *XMMzone::findSubZone(int zoneid) {
  return 0;
}

QString XMMzone::getName() {
  return name;
}

int XMMzone::getID() {
  return (id);
}
