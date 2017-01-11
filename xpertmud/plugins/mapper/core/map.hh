#ifndef XMUD_MAPPERMAP_H
#define XMUD_MAPPERMAP_H

#include "mobject.hh"
#include "zonemodel.h"

#include <QString>

class QCanvas;


class XMMmap:public QObject {
  Q_OBJECT
public:
  XMMmap(QObject *parent = 0, QString mapname=QString());
  ~XMMmap();
  QCanvas *canvas();
  XMZoneModel *getModel();

signals:
  void emitZoneAdded(QString zonename, int uniqueid);
  void emitChangeZone(int zoneid);

public slots:
  void slotAddRoom(int x, int y);
  void slotAddZone(QString zonename);
  void slotSelectZone(int zoneid);

protected:
  XMZoneModel *m_zoneModel;

}; 




#endif
