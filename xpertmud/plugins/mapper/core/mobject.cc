#include "mobject.hh"
#include "map.hh"

#define COPY_VALUES(X) { \
  m_name = X.m_name; \
  m_id = X.m_id; \
  m_type = X.m_type; \
}

XMObject::XMObject(QString objectName, XMType objectType, QString objectId) {
  if (objectName.isEmpty())
    m_name = QString("New object");
  else
    m_name = objectName;
  m_id = objectId;
  m_type = objectType;
}

XMObject::XMObject(XMObject &object) {
  COPY_VALUES(object);
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

XMObject &XMObject::operator=(const XMObject &right) {
  if (this == &right) {
    return *this;
  }

  COPY_VALUES(right)

  return *this;
}
