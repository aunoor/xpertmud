// -*- c++ -*-
#include "Factory.h"
#include "BattleCoreWidget.h"
#include "BattleMapWidget.h"
#include "BattleContacts.h"
#include "BattleHeat.h"
#include "BattleWeapons.h"
#include "BattleSpeed.h"
#include "BattleStatus.h"

int __kde_do_unload = 1;

QObject* BattleFactory::createObject(QObject* parent,
				     const char* name, 
				     const char* classname, 
				     const QStringList &args) {
  if (parent && parent->inherits("QWidget")) {
    QWidget * parentWidget=static_cast<QWidget *>(parent);
    if (QString(classname)=="BattleCoreWidget") {
      return new BattleCoreWidget(parentWidget,name,args);
    } else if (QString(classname)=="BattleContactWidget") {
      return new BattleContactWidget(parentWidget,name,args);
    } else if (QString(classname)=="BattleHeatWidget") {
      return new BattleHeatWidget(parentWidget,name,args);
    } else if (QString(classname)=="BattleSpeedWidget") {
      return new BattleSpeedWidget(parentWidget,name,args);
    } else if (QString(classname)=="BattleWeaponsWidget") {
      return new WeaponView(parentWidget,name,args);
    } else if (QString(classname)=="BattleStatusWidget") {
      return new BattleStatusWidget(parentWidget,name,args);
    } else { // Default action if QWidget is requested
    // TODO...
      return new BattleMapWidget(parentWidget,name,args);
    }
  }
  return NULL;
}




K_EXPORT_COMPONENT_FACTORY( xmud_battletech, BattleFactory )
