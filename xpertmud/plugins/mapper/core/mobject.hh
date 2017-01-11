#ifndef XMUD_MAPPERZONE_H
#define XMUD_MAPPERZONE_H

#include "core_global.h"
#include <QList>
#include <QMap>

class XMMmap;

class XMObject {
public:
  XMObject(XMMmap *parent, QString objectName=QString(), XMType objectType=XMTUnknown, int objectId=-1);
  virtual ~XMObject();

  QString getName();
  int getID();
  XMObject *findSubObject(int zoneid);

  XMType getType();

protected:
  int m_id;
  QString m_name;
  XMType m_type;
  QString m_flags;
  QMap<QString, QString> m_powers;
  XMObject *m_parent;
  QList<XMObject*> objects;
};

#endif
