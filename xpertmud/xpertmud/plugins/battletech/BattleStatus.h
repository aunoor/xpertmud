// -*- c++ -*-
#ifndef BATTLESTATUS_H
#define BATTLESTATUS_H
#include <qvariant.h>
#include <qwidget.h>
#include "Positions.h"
#include <qmap.h>

class BattleCore;
class MechInfo;

class BattleStatusDrawConfig;

class BattleStatusWidget : public QWidget {
  Q_OBJECT
public:
  BattleStatusWidget(QWidget *parent, const char *name, const QStringList& args);
  ~BattleStatusWidget();
  
  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  
  // </Plugin Interface> //////////////////////////////////////////////////////

protected slots:
  void slotUpdateMechInfo(const MechInfo &, const MechInfo &);
 
 protected:
  void paintEvent ( QPaintEvent * );
  BattleCore *core;
  SubHEXPos ownPosition;
  QString ownFlags;
  bool hasArmorStatus;
  QMap<QString,int> originalArmor;
  QMap<QString,int> currentArmor;
  QMap<QString,BattleStatusDrawConfig*> drawConfig;
  BattleStatusDrawConfig * currentDrawConfig;
};

#endif // #ifndef BATTLESTATUS_H
