#include "BattleContacts.h"
#include "BattleCore.h"
#include "MechInfo.h"
#include "BattleHeat.h"
#include <qlistview.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <klocale.h>

#include <iostream>
//////////////////////////////////////////////////////////////////////

class ContactItem:public QListViewItem {
public:
  ContactItem(QListView * parent, const MechInfo & mi, 
	      double dist, double bearing):
    QListViewItem(parent),
    bgColor(255,255,255),distance(dist)
  {
    setInfo(mi, dist, bearing);
  }  


  void setInfo(const MechInfo& mi, double dist, double bearing) {
    /*
    mi.scanStatus(),mi.getId(),mi.getName(),
		  QString("x: %1 y: %2 z: %3").arg(mi.getPos().getX(),3)
		  .arg(mi.getPos().getY(),3).arg(mi.getPos().getZ(),3),
		  QString("r: %1 b: %2").arg(dist,3,'f',1)
		  .arg(bearing,3,'f',0),
		  QString("s: %1 h: %2°").arg(mi.getSpeed(),3,'f',0)
		  .arg(mi.getHeadingDeg(),3,'f',0),
		  // TODO: v-Speed if applicable
		  mi.getStatus(),
		  QString::number(mi.age())),
    */
    setText(0, mi.scanStatus());
    setText(1, mi.getId());
    setText(2, mi.getName());
    setText(3, QString("x: %1 y: %2 z: %3").arg(mi.getPos().getX(),3)
	    .arg(mi.getPos().getY(),3).arg(mi.getPos().getZ(),3));
    setText(4, QString("r: %1 b: %2").arg(dist,3,'f',1)
	    .arg(bearing,3,'f',0));
    setText(5, QString("s: %1 h: %2°").arg(mi.getSpeed(),3,'f',0)
	    .arg(mi.getHeadingDeg(),3,'f',0));
    setText(6, mi.getStatus());
    setText(7, QString::number(mi.age()));

    if (mi.isEnemy()) {
      if (mi.getStatus().contains('D')) {
	bgColor=QColor(110,90,90);
      }else if (mi.age()==0) {
	bgColor=QColor(255,190,180);
      } else {
	bgColor=QColor(180,130,120);
      }
    } else {
      if (mi.getStatus().contains('D')) {
	bgColor=QColor(100,100,100);
      }else if (mi.age()==0) {
	bgColor=QColor(250,255,250);
      } else {
	bgColor=QColor(180,180,180);
      }
    }
  }

  void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) {
    QColorGroup ncg(cg);
    //Foreground, Button, Light, Midlight, Dark, Mid, Text, BrightText, ButtonText, Base, Background, Shadow, Highlight, HighlightedText, Link, LinkVisited,

    ncg.setBrush(QColorGroup::Base,bgColor);
    QListViewItem::paintCell(p,ncg,column,width,align);
  }

  QString key(int /* column */,bool /* asc */) const {

    // TODO: shuffle acording to column
    QString k;
    if (text(6).contains('D'))
      k+='Z';
    else
      k+='A';

    if (text(7).toInt()<60)
      k+='A';
    else
      k+='Z';

    if (text(1).upper()==text(1))
      k+='A';
    else
      k+='Z';

    k+=QString().sprintf("%06d",int(distance*100));
    //    std::cout  << "Generated KEY for " << text(2).latin1() << " : [" << k.latin1() << "]" <<std::endl;
    k+=text(1);
    return k;
  }
protected:
  QColor bgColor;
  double distance;
};

//////////////////////////////////////////////////////////////////////
BattleContactWidget::BattleContactWidget(QWidget *parent, 
				       const char *name, 
				       const QStringList& args):
  QWidget(parent,name),core(BattleCore::getInstance()) {

  connect(core, SIGNAL(queueMechInfoChange(const MechInfo&,
					   const MechInfo&)),
	  this, SLOT(slotUpdateMechInfo(const MechInfo&,
					const MechInfo&)));

  //  connect(core, SIGNAL(flushDisplayChange()),
  //	  this, SLOT(updateList()));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setAutoAdd(true);
  BattleHeatWidget * bh=new BattleHeatWidget(this,"HeatBar",QStringList());
  bh->setFixedHeight(15);
  listView=new QListView(this);
  //  listView->setPalette(QPalette(QColor(0,0,0),QColor(200,200,200)));


  listView->setFocusPolicy(NoFocus);
  listView->viewport()->setFocusPolicy(NoFocus);


  listView->addColumn(""); // SE 0
  listView->addColumn(""); // ID 1
  listView->addColumn("Name"); // 2
  listView->addColumn("Pos"); // 3
  listView->addColumn("Range"); // 4
  listView->addColumn("Movement"); // 5
  listView->addColumn(""); // Status 6
  listView->addColumn("Age"); // 7

  listView->setColumnAlignment(1,AlignHCenter);
  listView->setColumnAlignment(4,AlignHCenter);
  listView->setColumnAlignment(5,AlignHCenter);
  listView->setColumnAlignment(6,AlignHCenter);
  listView->setColumnAlignment(7,AlignRight);
  listView->setAllColumnsShowFocus(true);
  listView->setSelectionMode(QListView::NoSelection);
  connect(listView,SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)),
	  this,SLOT(popmeup(QListViewItem *, const QPoint &, int)));


  popup=new QPopupMenu(this);
  popup->insertItem(i18n("&Lock"),this, SLOT(slotPopup(int)),0,1);
  popup->insertItem(i18n("&Scan"),this, SLOT(slotPopup(int)),0,2);
  popup->insertItem(i18n("&Report"),this, SLOT(slotPopup(int)),0,3);
  popup->insertItem(i18n("&Face towards"),this, SLOT(slotPopup(int)),0,4);
  popup->insertItem(i18n("R&emove"),this, SLOT(slotPopup(int)),0,5);
  popup->insertSeparator();
  popup->insertItem(i18n("&Always show"),this, SLOT(slotPopup(int)),0,6);
  popup->insertItem(i18n("Always &hide"),this, SLOT(slotPopup(int)),0,7);
  popup->insertItem(i18n("Auto&matic"),this, SLOT(slotPopup(int)),0,8);
  

  setFocusPolicy(NoFocus);
}

BattleContactWidget::~BattleContactWidget() {
  BattleCore::returnInstance();
}

void BattleContactWidget::popmeup(QListViewItem *li, const QPoint &p, int) {
  if (li) {
    currentID=li->text(1);
    popup->exec(p);
  }
}

void  BattleContactWidget::slotPopup(int fun) {
  switch (fun) {
  case 1:
    core->slotSend("lock "+currentID+"\n");
    break;
  case 2:
    core->slotSend("scan "+currentID+"\n");
    break;
  case 3: {
    QVariant result;
    emit callback(0,currentID,result);
  }
    break;
  case 4: {
    MechInfo self=core->getMechInfo(core->getOwnId());
    MechInfo other=core->getMechInfo(currentID);
    SubPos dist = NormHexLayout::difference(self.getPos(),other.getPos());
    core->slotSend(QString("heading %1\n").arg(dist.getAngleDeg(),3,'f',0));
  }
    break;
  case 5:
    core->deleteMechInfo(currentID);
    break;
  case 6: // always show
	core->setVisibleOverride(currentID,true,true);
	break;
  case 7: //always hide
	core->setVisibleOverride(currentID,true,false);
	break;
  case 8: //automagic
	core->setVisibleOverride(currentID,false,true);
	break;
    break;
  default:
    break;
  }
}

void BattleContactWidget::slotFunctionCall(int func, const QVariant & args, 
					  QVariant & result) {
  // None usefull yet
}

void BattleContactWidget::slotUpdateMechInfo(const MechInfo & oldInfo, 
					     const MechInfo & mechInfo) {
  if(oldInfo.getId() == core->getOwnId() ||
     mechInfo.getId() == core->getOwnId()) {

    if(oldInfo.isValid() && items.find(oldInfo.getId().upper()) != items.end()) {
      delete items[oldInfo.getId().upper()];
      items.erase(oldInfo.getId().upper());
    }

    if(mechInfo.isValid() && items.find(mechInfo.getId().upper()) != items.end()) {
      delete items[mechInfo.getId().upper()];
      items.erase(mechInfo.getId().upper());
    }

    return;
  }

  if (mechInfo.isValid()) {
    MechInfo self = core->getMechInfo(core->getOwnId());
    SubPos dist = NormHexLayout::difference(self.getPos(),mechInfo.getPos());
    if(items.find(mechInfo.getId().upper()) != items.end()) {
      items[mechInfo.getId().upper()]->setInfo(mechInfo, dist.getDistance(),
				       dist.getAngleDeg());
    } else {
      ContactItem * i=new ContactItem
	(listView,mechInfo,dist.getDistance(),dist.getAngleDeg());
      items[mechInfo.getId().upper()] = i;
    }
    listView->sort();
  } else {
    if(items.find(oldInfo.getId().upper()) != items.end()) {
      delete items[oldInfo.getId().upper()];
      items.erase(oldInfo.getId().upper());
    }
    // sorting info isn't changed...
  }
}

/*
void BattleContactWidget::updateList() {
  listView->setUpdatesEnabled(false);
  MechInfo self=core->getMechInfo(core->getOwnId());
  QString marked;
  if (listView->selectedItem()!=NULL)
    marked=listView->selectedItem()->text(1).upper();
  listView->clear();
  for (BattleCore::mechIteratorT it=core->getMechBegin();
       it != core->getMechEnd(); ++it) {
    if (it->first.lower() != core->getOwnId())
      addMechInfo(it->second,self,it->first==marked);
  }    
  listView->setUpdatesEnabled(true);
  listView->triggerUpdate();
}
*/
