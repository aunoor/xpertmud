#include "BattleSpeed.h"
#include "BattleCore.h"
#include <qvariant.h>
#include <qwidget.h>
#include <qpainter.h>
#include <algorithm>
#ifndef WIN32
using std::max;
using std::min;
#else
#define max(x,y) ((x) > (y)?(x):(y))
#define min(x,y) ((x) > (y)?(y):(x))
#endif

BattleSpeedWidget::BattleSpeedWidget(QWidget *parent, 
				   const char *name, 
				   const QStringList& args):
  QWidget(parent,name,WResizeNoErase| WRepaintNoErase ),
  core(BattleCore::getInstance()), maxRunningSpeed(53.0), 
  maxWalkingSpeed(35.0),maxBackingSpeed(35.0),
  currentSpeed(0.0),lastScale(0.1),lastBack(35.0)
{
  connect(core,SIGNAL(speedChanged(double)),
	  this, SLOT(speedChanged(double)));

  connect(core,SIGNAL(desiredSpeedChanged(double)),
	  this, SLOT(desiredSpeedChanged(double)));

  connect(core,SIGNAL(maxSpeedChanged(double,double,double)),
	  this, SLOT(maxSpeedChanged(double,double,double)));

  // Connect here
}

BattleSpeedWidget::~BattleSpeedWidget() {
  BattleCore::returnInstance();
}


void BattleSpeedWidget::slotFunctionCall(int, const QVariant &, QVariant &) {
}

void BattleSpeedWidget::speedChanged(double speed) {
  currentSpeed=speed;
  update();
}

void BattleSpeedWidget::desiredSpeedChanged(double desiredSpd) {
  desiredSpeed = desiredSpd;
  update();
}

void BattleSpeedWidget::maxSpeedChanged(double maxRunningSpd, 
					double maxWalkingSpd, 
					double maxBackingSpd) {
  maxRunningSpeed = maxRunningSpd;
  maxWalkingSpeed = maxWalkingSpd;
  maxBackingSpeed = fabs(maxBackingSpd);
  update();
}



void BattleSpeedWidget::paintEvent ( QPaintEvent *e ) {
  QRect bound = rect();
  // vertical
  double mymr= max(maxRunningSpeed,currentSpeed);
  double mymb;
  if(currentSpeed < 0.0)
    mymb = max(maxBackingSpeed, currentSpeed*-1);
  else
    mymb = maxBackingSpeed;
  QPainter painter;
  painter.begin(this);
  QFontMetrics fm = painter.fontMetrics();
  QRect fb = fm.boundingRect("0123456789-.");
  if(mymr < 1.0) {
    painter.setPen(QColor(150,150,0));
    painter.setBrush(QColor(150,150,0));
    painter.drawRect(0,0,bound.width(),bound.height());
    painter.setPen(QPen(QColor(0,0,0),5));
    painter.drawLine(0,0,bound.width(),bound.height());
    painter.drawLine(bound.width(),0 ,0,bound.height());
    painter.end();
    return;
  }
  int offset = (fb.height()+1)/2 +1;
  double scale = ((double)(bound.height()-2*offset))/(mymr + mymb);
  int speedY =  bound.height() - int((mymb+currentSpeed) * scale) - offset-3;
  int desireY =  bound.height() - int((mymb+desiredSpeed) * scale) - offset-4;
  int backY = bound.height() - offset;
  int stopY = bound.height() - int(mymb * scale) - offset;
  int walkY = bound.height() - int((mymb + maxWalkingSpeed)*scale)- offset;
  int runY = offset;
  lastScale = scale;
  lastBack = mymb;
  /*
  painter.setPen(QColor(0,0,0));
  painter.drawLine(0,walkY,bound.width(),walkY);
  painter.setPen(QColor(255,0,0));
  painter.drawLine(0,stopY,bound.width(),stopY);
  painter.setPen(QColor(0,255,0));
  painter.drawLine(0,runY,bound.width(),runY);
  painter.setPen(QColor(150,150,0));
  */

  int plength =max(0,bound.width()*2 / 5 - 3);

  painter.setBrush(QColor(255,100,0));
  painter.setPen(QColor(255,100,0));
  painter.drawRect(0,0,bound.width(),runY);
  painter.setBrush(QColor(255,130,0));
  painter.setPen(QColor(255,130,0));
  painter.drawRect(0,runY,bound.width(),walkY-runY);
  painter.setBrush(QColor(220,150,0));
  painter.setPen(QColor(220,150,0));
  painter.drawRect(0,walkY,bound.width(),stopY-walkY);
  painter.setBrush(QColor(170,170,0));
  painter.setPen(QColor(170,170,0));
  painter.drawRect(0,stopY,bound.width(),backY-stopY);
  painter.setBrush(QColor(150,150,0));
  painter.setPen(QColor(150,150,0));
  painter.drawRect(0,backY,bound.width(),bound.height()-backY);

  painter.setBrush(QColor(0,0,0));
  painter.setPen(QColor(0,0,0));
  QPointArray pt2(3);
  pt2[0] = QPoint(bound.width(),desireY);
  pt2[1] = QPoint(bound.width()-(plength*3 /4), desireY+4);
  pt2[2] = QPoint(bound.width(), desireY+8);
#if QT_VERSION<300
  painter.drawPolygon(pt2);
#else
  painter.drawConvexPolygon(pt2);
#endif


  QPointArray pt(5);
  pt[0] = QPoint(bound.width(),speedY);
  pt[1] = QPoint(bound.width()-plength, speedY);
  pt[2] = QPoint(bound.width()-plength-3, speedY+3);
  pt[3] = QPoint(bound.width()-plength, speedY+6);
  pt[4] = QPoint(bound.width(),speedY+6);
  painter.setBrush(QColor(180,0,0));
  painter.setPen(QColor(0,0,0));
  

#if QT_VERSION<300
  painter.drawPolygon(pt);
#else
  painter.drawConvexPolygon(pt);
#endif
  QString sstop = "0";
  QString swalk = QString::number(int(maxWalkingSpeed));
  QString srun  = QString::number(int(mymr));
  QString sback = QString::number(-1*int(mymb));
  
  int sto = fm.boundingRect(sstop).width();
  int maxout = rect().width()/2;
  int stw = fm.boundingRect(swalk).width();
  //  maxout = max(maxout, stw);
  int str = fm.boundingRect(srun).width();
  //maxout = max(maxout, str);
  int stb = fm.boundingRect(sback).width();
  //maxout = max(maxout, stb);

  painter.drawText(maxout-sto/2,stopY+offset-1,sstop);
  painter.drawText(maxout-stw/2,walkY+offset-1,swalk);
  painter.drawText(maxout-str/2,runY+offset-1,srun);
  painter.drawText(maxout-stb/2,backY+offset-1,sback);


  painter.end();

}


void BattleSpeedWidget::mouseReleaseEvent ( QMouseEvent * e ) {
  double d = ((rect().height()-e->y()-3)/lastScale)-lastBack;
  // cout << "e.y(): " << e->y() << " rect.height() " << rect().height() << " lastBack:" << lastBack << " lastScale: " << lastScale << endl;
  QString str;
  QTextStream stream(str,IO_WriteOnly);
  stream << "speed ";
  if(fabs(d) < 5.0)
    stream << "stop" << endl;
  else if(fabs(d-maxRunningSpeed) < 5.0)
    stream << "run" << endl;
  else if(fabs(d-maxWalkingSpeed) < 5.0)
    stream << "walk" << endl;
  else if(fabs(d+maxBackingSpeed) < 5.0)
    stream << "back" << endl;
  else if(d < 0)
    stream << max(d, maxBackingSpeed*-1.0) << endl;
  else
    stream << min(d, maxRunningSpeed) << endl;
  core->slotSend(str);
  emit changeSpeed(d);
}
