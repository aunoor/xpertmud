#include "zonemodel.h"

XMZoneModel::XMZoneModel(QObject *parent) : QAbstractItemModel(parent) {
  m_next_zone_id = 0;
}

XMZoneModel::~XMZoneModel() {
  qDeleteAll(m_objects);
  m_objects.clear();
}

QVariant XMZoneModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case 0:
          return QStringLiteral("Id");
        case 1:
          return QStringLiteral("Name");
        default:
          return QVariant();
      }
    }//if DisplayRole

    if (role == Qt::TextAlignmentRole) {
      return Qt::AlignHCenter;
    }//if TextAlignment
  }//if horizontal
  return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex XMZoneModel::index(int row, int column, const QModelIndex &parent) const {
  if (!parent.isValid()) {
    if (row>=m_objects.count()) return QModelIndex();
    if (column>=columnCount(parent)) return QModelIndex();
    return createIndex(row, column);
  }
  return QModelIndex();
}

QModelIndex XMZoneModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

int XMZoneModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) return m_objects.count();
  return 0;
}

int XMZoneModel::columnCount(const QModelIndex &parent) const {
  if (!parent.isValid()) return 2;
  return 0;
}

QVariant XMZoneModel::data(const QModelIndex &index, int role) const {

  if (role == Qt::DisplayRole) {

    XMObject *tmpObject;

    if (!index.parent().isValid()) {

      tmpObject = m_objects.at(index.row());

      switch (index.column()) {
        case 0:
          return tmpObject->getID();
        case 1:
          return tmpObject->getName();
        default:
          return QVariant();
      }
    }
  }

  return QVariant();
}

XMObject *XMZoneModel::findObject(QString id) {
    foreach(XMObject *tmpObject, m_objects) {
      if (tmpObject->getID() == id) return tmpObject;
      XMObject *subZone = tmpObject->findSubObject(id);
      if (subZone != NULL) return subZone;
    }
  return NULL;
}

XMObject *XMZoneModel::findObject(XMObject *object) {
    foreach(XMObject *tmpObject, m_objects) {
      if (tmpObject->getID()==object->getID() && tmpObject->getType()==object->getType())
        return tmpObject;
      if (object->getType()!=XMTRoom) {
        //save some cpu time, because Room always is top level now
        XMObject *subObject = tmpObject->findSubObject(object);
        if (subObject != NULL) return subObject;
      }
    }
  return NULL;
}


void XMZoneModel::addNewRoom(XMObject *object) {
  XMObject * tmpObj = findObject(object->getID());
  if (tmpObj==NULL) {
    beginInsertRows(QModelIndex(),m_objects.count(), m_objects.count());
    m_objects.append(object);
    endInsertRows();
  } else {
    int idx = m_objects.indexOf(tmpObj);
    m_objects.replace(idx, object);
    emit dataChanged(index(idx,0,QModelIndex()),index(idx,columnCount(QModelIndex())-1,QModelIndex()));
    delete tmpObj;
  }
}
