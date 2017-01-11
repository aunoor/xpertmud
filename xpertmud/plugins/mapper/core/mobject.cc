#include "mobject.hh"
#include "map.hh"

XMObject::XMObject(QString objectName, XMType objectType, QString objectId) {
  if (objectName.isEmpty())
    m_name = QString("New object");
  else
    m_name = objectName;
  m_id = objectId;
  m_type = objectType;
}

XMObject::~XMObject() {

}

XMObject *XMObject::findSubObject(QString id) {
  return 0;
}

XMObject *XMObject::findSubObject(XMObject *object) {
  return nullptr;
}

QString XMObject::getName() {
  return m_name;
}

QString XMObject::getID() {
  return m_id;
}

XMType XMObject::getType() {
  return m_type;
}
