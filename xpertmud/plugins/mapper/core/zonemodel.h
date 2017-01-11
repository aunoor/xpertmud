#ifndef XMUD_ZONEMODEL_H
#define XMUD_ZONEMODEL_H

#include "mobject.hh"
#include <QAbstractItemModel>

class XMZoneModel : public QAbstractItemModel {
public:

    XMZoneModel(QObject *parent);
    virtual ~XMZoneModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    XMObject *findObject(QString id);
    XMObject *findObject(XMObject *object);

    void addNewRoom(XMObject* object);

private:
    QList<XMObject*> m_objects;

    int m_next_zone_id;

};

#endif //XMUD_ZONEMODEL_H
