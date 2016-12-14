#ifndef XMUD_MAPPERMAP_H
#define XMUD_MAPPERMAP_H
#include <qobject.h>
#include "zone.hh"
#include <qlist.h>
#include <qstring.h>

class QCanvas;


class XMMmap:public QObject {
  Q_OBJECT
public:
  XMMmap(QObject *parent, const char *name, const char *mapname);
  ~XMMmap();
  QCanvas *canvas();

signals:
  void emitZoneAdded(QString zonename, int uniqueid);
  void emitChangeZone(int zoneid);

public slots:
  void slotAddRoom(int x, int y);
  void slotAddZone(QString *zonename);
  void slotSelectZone(int zoneid);

protected:
  XMMzone *findZone(int zoneid);
  QList<XMMzone> zones;
  XMMzone *zone;
  int next_zone_id;
}; 




#endif
