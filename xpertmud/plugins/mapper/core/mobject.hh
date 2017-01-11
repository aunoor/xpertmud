#ifndef XMUD_MAPPERZONE_H
#define XMUD_MAPPERZONE_H

#include "core_global.h"
#include <QList>
#include <QMap>

class XMMmap;

class XMObject {
public:
  XMObject(QString objectName=QString(), XMType objectType=XMTUnknown, QString objectId=QString());
  virtual ~XMObject();

  QString getName();
  QString getID();
  XMObject *findSubObject(QString id);
  XMObject *findSubObject(XMObject *object);

  XMType getType();

protected:
  QString m_id;
  QString m_name;
  XMType m_type;
  QString m_flags;
  QMap<QString, QString> m_powers;
  XMObject *m_parent;
  QList<XMObject*> objects;
};

#endif
