// -*- c++ -*-
#ifndef BATTLESPEED_H
#define BATTLESPEED_H
#include <qvariant.h>
#include <qwidget.h>


class BattleCore;

class BattleSpeedWidget : public QWidget {
  Q_OBJECT
public:
  BattleSpeedWidget(QWidget *parent, const char *name, const QStringList& args);
  ~BattleSpeedWidget();
  
  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

  // Request a speed change
  void changeSpeed(double speed);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  
  // </Plugin Interface> //////////////////////////////////////////////////////

protected slots:
  // not implemented yet, but maybe I will some day
  // false = horizontal
  // true = vertical
  //  void setMode(bool horizontal);
  void speedChanged(double speed);
  void desiredSpeedChanged(double desiredSpeed);
  void maxSpeedChanged(double maxRunningSpeed, 
		       double maxWalkingSpeed, 
		       double maxBackingspeed);
 
 protected:
  void mouseReleaseEvent ( QMouseEvent * e );
  void paintEvent ( QPaintEvent * );
  BattleCore *core;
  double maxRunningSpeed;
  double maxWalkingSpeed;
  double maxBackingSpeed;
  double currentSpeed;
  double desiredSpeed;
  double lastScale;
  double lastBack;
  //  double lastRun;
};

#endif // #ifndef BATTLESPEED_H
