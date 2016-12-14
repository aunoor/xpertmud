

#include "BattleHeat.h"
#include "BattleCore.h"
#include <qvariant.h>
#include <qwidget.h>
#include <qpainter.h>

BattleHeatWidget::BattleHeatWidget(QWidget *parent, 
				   const char *name, 
				   const QStringList& args):
  QWidget(parent,name,WResizeNoErase| WRepaintNoErase ),
  core(BattleCore::getInstance()), heatDissipation(0), heat(0)
{
  connect(core,SIGNAL(heatChanged(int)),
	  this, SLOT(heatChanged(int)));
  connect(core,SIGNAL(heatDissipationChanged(int)),
	  this, SLOT(heatDissipationChanged(int)));
}

BattleHeatWidget::~BattleHeatWidget() {
  BattleCore::returnInstance();
}


void BattleHeatWidget::slotFunctionCall(int, const QVariant &, QVariant &) {
}

void BattleHeatWidget::heatChanged(int nheat) {
  heat = nheat;
  update();
}

void BattleHeatWidget::heatDissipationChanged(int nheatDissipation) {
  heatDissipation = nheatDissipation;
  update();
}


void BattleHeatWidget::paintEvent ( QPaintEvent *p ) {
  QRect size = rect();
  double greyRange = (double)(heatDissipation * 10);
  double scale = greyRange + 300.0;
  int greyPix = (int)((double)(size.width())*greyRange/scale);
  int darkGreenPix = (int)((double)(size.width())*(greyRange+90)/scale);
  int lightGreenPix = (int)((double)(size.width())*(greyRange+140)/scale);
  int yellowPix = (int)((double)(size.width())*(greyRange+190)/scale);
  int heatInt = (int)((double)heat * (double)size.width() / scale);
  QColor grey1(0,0,0);
  QColor grey2(130,130,130);
  QColor dgreen1(0,80,0);
  QColor dgreen2(0,230,0);
  QColor lgreen1(0,100,0);
  QColor lgreen2(0,250,0);
  QColor yellow1(100,100,0);
  QColor yellow2(230,230,0);
  QColor red1(100,0,0);
  QColor red2(230,0,0);

  QPainter painter;
  painter.begin(this);
  if(heatInt < greyPix) {
    painter.setPen(grey2);
    painter.setBrush(grey2);
    painter.drawRect(0,0,heatInt,size.height());
    painter.setPen(grey1);
    painter.setBrush(grey1);
    painter.drawRect(heatInt,0,greyPix-heatInt,size.height());
  } else {
    painter.setPen(grey2);
    painter.setBrush(grey2);
    painter.drawRect(0,0,greyPix,size.height());
  }

  if(heatInt <= greyPix) {
    painter.setPen(dgreen1);
    painter.setBrush(dgreen1);
    painter.drawRect(greyPix,0,darkGreenPix-greyPix,size.height());
  } else if(heatInt < darkGreenPix) {
    painter.setPen(dgreen2);
    painter.setBrush(dgreen2);
    painter.drawRect(greyPix,0 ,heatInt-greyPix,size.height());
    painter.setPen(dgreen1);
    painter.setBrush(dgreen1);
    painter.drawRect(heatInt,0 ,darkGreenPix-heatInt,size.height());
  } else {
    painter.setPen(dgreen2);
    painter.setBrush(dgreen2);
    painter.drawRect(greyPix,0,darkGreenPix-greyPix,size.height());
  }


  if(heatInt <= darkGreenPix) {
    painter.setPen(lgreen1);
    painter.setBrush(lgreen1);
    painter.drawRect(darkGreenPix,0,lightGreenPix-darkGreenPix,size.height());
  } else if(heatInt < lightGreenPix) {
    painter.setPen(lgreen2);
    painter.setBrush(lgreen2);
    painter.drawRect(darkGreenPix,0 ,heatInt-darkGreenPix,size.height());
    painter.setPen(lgreen1);
    painter.setBrush(lgreen1);
    painter.drawRect(heatInt,0,lightGreenPix-heatInt,size.height());
  } else {
    painter.setPen(lgreen2);
    painter.setBrush(lgreen2);
    painter.drawRect(darkGreenPix,0,lightGreenPix-darkGreenPix,size.height());
  }

  if(heatInt <= lightGreenPix) {
    painter.setPen(yellow1);
    painter.setBrush(yellow1);
    painter.drawRect(lightGreenPix,0,yellowPix-lightGreenPix,size.height());
  } else if(heatInt < yellowPix) {
    painter.setPen(yellow2);
    painter.setBrush(yellow2);
    painter.drawRect(lightGreenPix,0 ,heatInt-lightGreenPix,size.height());
    painter.setPen(yellow1);
    painter.setBrush(yellow1);
    painter.drawRect(heatInt,0,yellowPix-heatInt,size.height());
  } else {
    painter.setPen(yellow2);
    painter.setBrush(yellow2);
    painter.drawRect(lightGreenPix,0,yellowPix-lightGreenPix,size.height());
  }


  if(heatInt <= yellowPix) {
    painter.setPen(red1);
    painter.setBrush(red1);
    painter.drawRect(yellowPix,0,size.width()-yellowPix,size.height());
  } else {
    painter.setPen(red2);
    painter.setBrush(red2);
    painter.drawRect(yellowPix,0 ,heatInt-yellowPix,size.height());
    painter.setPen(red1);
    painter.setBrush(red1);
    painter.drawRect(heatInt,0,size.width()-heatInt ,size.height());
  } 
    
  painter.end();
}
