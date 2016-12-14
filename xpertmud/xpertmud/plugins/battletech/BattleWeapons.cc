#include "BattleWeapons.h"
#include "BattleCore.h"

#include <qlistview.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <klocale.h>
#include <cassert>

class WeaponViewItem: public QListViewItem {
public:
  WeaponViewItem(QListView* parent, const WeaponInfo& wi, const WeaponStat &stat): 
    QListViewItem(parent), heatColor(0, 0, 0), textColor("lightgrey")
  {
    setInfo(wi,stat,0,30);
  }

  void setInfo(const WeaponInfo& wi, const WeaponStat &stat, int heat,
	       int heatDissipation) {
    setText(0, QString::number(wi.getId()));
    setText(1, wi.getSection());
    if(wi.hasTechName())
      setText(2, wi.getTechName() + "." + wi.getName());
    else
      setText(2, wi.getName());
/*
    if(wi.hasBth()) {
      setText(3, QString::number(wi.getBth()));
    } else {
      setText(3, "n/a");
    }
*/

    if(wi.isRecycling()) {
      textColor = QColor("grey");
      setText(4, QString::number(wi.getRecycleTime()));
    } else {
      textColor = QColor("lightgrey");
      setText(4, wi.getStatus());
    }

    setText(5, wi.getAmmoType());

    if(stat.hasHeat()) {
      setText(6, QString::number(double(stat.getHeat())/10.0));
      int newHeat = heat + stat.getHeat();
      if(newHeat < heatDissipation*10) 
	heatColor = QColor(0, 0, 0);
      else if(newHeat < heatDissipation*10 + 90)
	heatColor = QColor(0, 80, 0);
      else if(newHeat < heatDissipation*10 + 140)
	heatColor = QColor(0, 100, 0);
      else if(newHeat < heatDissipation*10 + 190)
	heatColor = QColor(100, 100, 0);
      else
	heatColor = QColor(100, 0, 0);
    } else {
      setText(6, "n/a");
    }

    if(stat.hasMinRange() && stat.hasShortRange() &&
       stat.hasMidRange() && stat.hasLongRange()) {
      QString ranges;
      QTextStream str(&ranges, IO_WriteOnly);
      str << stat.getMinRange() << " "
	  << stat.getShortRange() << " "
	  << stat.getMidRange() << " "
	  << stat.getLongRange();
      setText(7, ranges);
    } else {
      setText(7, "n/a");
    }

    if(stat.hasDamage()) {
      setText(8, QString::number(stat.getDamage()));
    } else {
      setText(8, "n/a");
    }
    repaint();
  }

  void paintCell(QPainter* p, const QColorGroup& cg, int column, int width,
		 int align) {
    QColorGroup newGroup(cg);
    newGroup.setColor(QColorGroup::Text, textColor);
    newGroup.setBrush(QColorGroup::Base, heatColor);
    QListViewItem::paintCell(p, newGroup, column, width, align);
  }

private:
  QColor heatColor;
  QColor textColor;
};

//////////////////////////////////////////////////////////////////////

WeaponView::WeaponView(QWidget* parent, const char* name, const QStringList& /*args*/):
  QWidget(parent, name), core(BattleCore::getInstance())
{
  connect(core, SIGNAL(weaponChange(int)),
	  this, SLOT(slotUpdateEntry(int)));
  connect(core, SIGNAL(nrWeaponsChange(int)),
	  this, SLOT(slotNrWeaponsChange(int)));
  connect(core,SIGNAL(heatChanged(int)),
	  this, SLOT(heatChanged(int)));
  connect(core,SIGNAL(heatDissipationChanged(int)),
	  this, SLOT(heatDissipationChanged(int)));
  QVBoxLayout *l = new QVBoxLayout(this);
  l->setAutoAdd(true);

  listView=new QListView(this);

#if QT_VERSION < 300
  listView->setBackgroundColor(QColor(0,0,0));
#else
  listView->setPaletteBackgroundColor(QColor(0,0,0));
#endif

  listView->setFocusPolicy(NoFocus);
  listView->viewport()->setFocusPolicy(NoFocus);
  
  listView->addColumn(""); // 0  Nr
  listView->addColumn(""); // 1  Section
  listView->addColumn("Name"); // 2
  listView->addColumn(""); // 3, was BTH, remove sometimes
  listView->addColumn(""); // 4 Status
  listView->addColumn(""); // 4 Ammo Type
  listView->addColumn("H"); // 4 Ammo Type
  listView->addColumn("Ranges"); // 5  
  listView->addColumn("D"); // 4 Ammo Type
}

WeaponView::~WeaponView() {
  BattleCore::returnInstance();
}

void WeaponView::slotFunctionCall(int /*func*/, const QVariant & /*args*/, QVariant & /*result*/) {
}

void WeaponView::slotUpdateEntry(int id) {
  assert(id>=0);
  WeaponInfo wi=core->getWeaponInfo(id);
  WeaponStat ws=core->getWeaponStat(wi.key());
  if((int)items.size() <= id)
    items.resize(id+1);
  if(items[id] == NULL) {
    items[id] = new WeaponViewItem(listView, wi, ws);
  } else {
    items[id]->setInfo(wi,ws, heat, heatDissipation);
  }
}

void WeaponView::slotNrWeaponsChange(int nr) {
  assert(nr>=0);
  for(unsigned int i=nr; i<items.size(); ++i) {
    if(items[i] != NULL)
      delete items[i];
  }
  items.resize(nr);
}

void WeaponView::heatChanged(int nheat) {
  heat = nheat;
  updateEntries();
}

void WeaponView::heatDissipationChanged(int nheatDissipation) {
  heatDissipation = nheatDissipation;
  updateEntries();
}

void WeaponView::updateEntries() {
  for(unsigned int i=0; i<items.size(); ++i) {
    slotUpdateEntry(i);
  }
}
