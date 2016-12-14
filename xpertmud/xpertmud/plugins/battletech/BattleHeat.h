// -*- c++ -*-
#ifndef BATTLEHEAT_H
#define BATTLEHEAT_H
#include <qvariant.h>
#include <qwidget.h>


class BattleCore;

class BattleHeatWidget : public QWidget {
  Q_OBJECT
public:
  BattleHeatWidget(QWidget *parent, const char *name, const QStringList& args);
  ~BattleHeatWidget();
  
  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  
  // </Plugin Interface> //////////////////////////////////////////////////////

protected slots:
  void heatChanged(int heat);
  void heatDissipationChanged(int heatDissipation);

protected:
  void paintEvent ( QPaintEvent * );
  BattleCore *core;
  int heatDissipation;
  int heat;
};

#endif // #ifndef BATTLEHEAT_H
