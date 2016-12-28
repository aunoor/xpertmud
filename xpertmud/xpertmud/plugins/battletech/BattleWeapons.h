// -*- c++ -*-
#ifndef BATTLEWEAPONS_H
#define BATTLEWEAPONS_H

#include "WeaponInfo.h"

#include <QWidget>
#include <QAbstractItemModel>
#include <vector>

class BattleCore;
class QTreeView;
class QMenu;

class WeaponViewItem;

class WeaponsModel: public QAbstractItemModel {

Q_OBJECT

public:
    WeaponsModel(QObject *parent);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void updateWeaponInfo(int id);
    void changeNrWeapons(int count);
    void setHeat(int heat);
    void setHeatDissipation(int heatDissipation);



private:
    BattleCore * core;
    QVector<WeaponInfo> m_weapons;
    int heatDissipation;
    int heat;
    int getIndex(const WeaponInfo &info);
};


class WeaponView: public QWidget {
  Q_OBJECT
public:
  WeaponView(QWidget *parent, const char* name, const QStringList& args);
  ~WeaponView();
  
  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface> //////////////////////////////////////////////////////

protected slots:
  void slotUpdateEntry(int);
  void slotNrWeaponsChange(int);
  void heatChanged(int heat);
  void heatDissipationChanged(int heatDissipation);

private:
  void updateEntries();

  BattleCore* core;
  QTreeView* listView;
  QMenu* popup;
  std::vector<WeaponViewItem *> items;
  WeaponsModel *weapons_model;

  // for heat calculations:
  //int heatDissipation;
  //int heat;

};

// TODO:
//class AmmoView;
//class ArmsNLegsView;

/**
 * This class combines the different view classes into
 * one nice toplevel widget.
class BattleWeaponWidget: public QWidget {
  Q_OBJECT
public:
  BattleWeaponWidget(QWidget *parent, const char* name, const QStringList& args);
  ~BattleWeaponWidget();

  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface> //////////////////////////////////////////////////////

private:
  WeaponView* weaponView;
};
*/

#endif
