#ifndef XMUD_MAPPERZONE_H
#define XMUD_MAPPERZONE_H
#include <qlist.h>
class QCanvas;
class QString;
class XMMmap;

class XMMzone {
public:
  XMMzone(XMMmap *parent, QString *zonename, int zoneid);
  ~XMMzone();
  QString getName();
  int getID();
  XMMzone *findSubZone(int zoneid);

  QCanvas *canvas;


protected:
  QString *name;
  int id;
  int level;
  int next_room_id;
  QList<XMMzone> zones;
};

#endif
