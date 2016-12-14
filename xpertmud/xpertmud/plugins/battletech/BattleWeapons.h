// -*- c++ -*-
#ifndef BATTLEWEAPONS_H
#define BATTLEWEAPONS_H

#include <qwidget.h>
#include <qvariant.h>
#include <vector>

class BattleCore;
class QListView;
class QPopupMenu;

class WeaponViewItem;
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
  QListView* listView;
  QPopupMenu* popup;
  std::vector<WeaponViewItem *> items;

  // for heat calculations:
  int heatDissipation;
  int heat;
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
