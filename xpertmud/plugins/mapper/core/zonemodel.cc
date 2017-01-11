#include "zonemodel.h"

XMZoneModel::XMZoneModel(QObject *parent) : QAbstractItemModel(parent) {
  m_next_zone_id = 0;
}

XMZoneModel::~XMZoneModel() {
  qDeleteAll(m_zones);
  m_zones.clear();
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
    if (row>=m_zones.count()) return QModelIndex();
    if (column>=columnCount(parent)) return QModelIndex();
    return createIndex(row, column);
  }
  return QModelIndex();
}

QModelIndex XMZoneModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

int XMZoneModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) return m_zones.count();
  return 0;
}

int XMZoneModel::columnCount(const QModelIndex &parent) const {
  if (!parent.isValid()) return 2;
  return 0;
}

QVariant XMZoneModel::data(const QModelIndex &index, int role) const {
  return QVariant();
}

XMObject *XMZoneModel::findZone(int zoneid) {
          foreach(XMObject *tmpZone, m_zones) {
      if (tmpZone->getID() == zoneid) return tmpZone;
      XMObject *subZone = tmpZone->findSubObject(zoneid);
      if (subZone != NULL) return subZone;
    }
  return NULL;
}