#include "mobject.hh"
#include "map.hh"

XMObject::XMObject(XMMmap *parent, QString objectName, XMType objectType, int objectId) {
  if (objectName.isEmpty())
    m_name = QString("New object");
  else
    m_name = objectName;
  m_id = objectId;
  m_type = objectType;
}

XMObject::~XMObject() {

}

XMObject *XMObject::findSubObject(int zoneid) {
  return 0;
}

QString XMObject::getName() {
  return m_name;
}

int XMObject::getID() {
  return m_id;
}

XMType XMObject::getType() {
  return m_type;
}
