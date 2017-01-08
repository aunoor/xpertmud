#ifndef XMUD_MAPPERZONE_H
#define XMUD_MAPPERZONE_H

#include <QList>

//class QCanvas;
//class QString;
class XMMmap;

//dummy class for just compile
class QCanvas {

};

class XMMzone {
public:
  XMMzone(XMMmap *parent, QString zonename=QString(), int zoneid=0);
  virtual ~XMMzone();
  QString getName();
  int getID();
  XMMzone *findSubZone(int zoneid);

  QCanvas *canvas;


protected:
  QString name;
  int id;
  int level;
  int next_room_id;
  QList<XMMzone> zones;
};

#endif
