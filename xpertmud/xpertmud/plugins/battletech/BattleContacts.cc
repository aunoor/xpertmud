#include "BattleContacts.h"
#include "BattleCore.h"
#include "MechInfo.h"
#include "BattleHeat.h"
#include <QTreeWidget>
#include <QLayout>
#include <QMenu>
#include <QHeaderView>
#include <QDebug>
#include <klocale.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////

ContactsModel::ContactsModel(QObject *parent) : QAbstractItemModel(parent) {
  core = BattleCore::getInstance();
}

int ContactsModel::rowCount(const QModelIndex &parent) const {
  return m_contacts.count();
}

int ContactsModel::columnCount(const QModelIndex &parent) const {
  return 8;
}

QModelIndex ContactsModel::index(int row, int column, const QModelIndex &parent) const {
  if (row>=m_contacts.count()) return QModelIndex();
  if (column>=8) return QModelIndex();
  if (parent.isValid()) return QModelIndex();
  return createIndex(row, column );
}

QModelIndex ContactsModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

QVariant ContactsModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case 0:
        case 1:
          return QString("");
          break;
        case 2:
          return QString("Name");
        case 3:
          return QString("Pos");
        case 4:
          return QString("Range");
        case 5:
          return QString("Movement");
        case 6:
          return QString("");
        case 7:
          return QString("Age");
        default:
          return QVariant();
      }
    }//if DisplayRole

    if (role == Qt::SizeHintRole) {
      switch (section) {
        case 1: {
          QTreeView *tv = dynamic_cast<QTreeView*>(QObject::parent());
          if (tv==NULL) return QVariant();
          QFontMetrics fm = tv->header()->fontMetrics();
          return QSize(fm.maxWidth()*2+4, fm.height());
        }
        default:
          return QVariant();
      }
    }

    if (role == Qt::TextAlignmentRole) {
      return Qt::AlignHCenter;
    }//if TextAlignment

  }//if horizontal

  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const {

  if (role == Qt::TextAlignmentRole) {
    switch (index.column()) {
      case 1:
      case 4:
      case 5:
      case 6:
        return Qt::AlignHCenter;
      case 7:
        return Qt::AlignRight;
      default:
        return Qt::AlignLeft;
    }
  }//if TextAlignment

  const MechInfo *mi = &(m_contacts.at(index.row()));

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0:
        return mi->scanStatus();
      case 1:
        return mi->getId();
      case 2:
        return mi->getName();
      case 3:
        return QString("x: %1 y: %2 z: %3").arg(mi->getPos().getX(),3)
                .arg(mi->getPos().getY(),3).arg(mi->getPos().getZ(),3);
      case 4: {
        MechInfo self = core->getMechInfo(core->getOwnId());
        SubPos dist = NormHexLayout::difference(self.getPos(), mi->getPos());
        return QString("r: %1 b: %2").arg(dist.getDistance(),3,'f',1).arg(dist.getAngleDeg(),3,'f',0);
      }
      case 5:
        return QString("s: %1 h: %2").arg(mi->getSpeed(),3,'f',0)
                .arg(mi->getHeadingDeg(),3,'f',0);
      case 6:
        return mi->getStatus();
      case 7:
        return mi->age();
      default:
        QVariant();
    }
  } //if DisplayRole

  if (role == Qt::BackgroundRole) {
    QColor bgColor;
    if (mi->isEnemy()) {
      if (mi->getStatus().contains('D')) {
        bgColor=QColor(110,90,90);
      }else if (mi->age()==0) {
        bgColor=QColor(255,190,180);
      } else {
        bgColor=QColor(180,130,120);
      }
    } else {
      if (mi->getStatus().contains('D')) {
        bgColor=QColor(100,100,100);
      }else if (mi->age()==0) {
        bgColor=QColor(250,255,250);
      } else {
        bgColor=QColor(180,180,180);
      }
    }
    return bgColor;
  }//if BackgroundRole

  return QVariant();
}

int ContactsModel::getIndex(const MechInfo &info) {
  uint hash = info.getIdHash();
  for (int i=0; i<m_contacts.count(); i++) {
    if (m_contacts.at(i).getIdHash() == hash) return i;
  }
  return -1;
}

void ContactsModel::updateMechInfo(const MechInfo &oldInfo, const MechInfo &mechInfo) {

  if ((oldInfo.getId() == core->getOwnId()) || (mechInfo.getId() == core->getOwnId())) {
    int idx = getIndex(oldInfo);
    if (oldInfo.isValid() && (idx > -1)) {
      beginRemoveRows(QModelIndex(), idx, idx);
      m_contacts.remove(idx);
      endRemoveRows();
    }

    idx = getIndex(mechInfo);
    if (mechInfo.isValid() && (idx > -1)) {
      beginRemoveRows(QModelIndex(), idx, idx);
      m_contacts.remove(idx);
      endRemoveRows();
    }
    return;
  }

  if (mechInfo.isValid()) {
    int idx = getIndex(mechInfo);
    if (idx != -1) {
      m_contacts.replace(idx, mechInfo);
      emit dataChanged(index(idx, 0, QModelIndex()), index(idx, 7, QModelIndex()),
                       QVector<int>() << (int) Qt::DisplayRole);
    } else {
      beginInsertRows(QModelIndex(), m_contacts.count(), m_contacts.count());
      m_contacts.append(mechInfo);
      endInsertRows();
    }
  } else {
    int idx = getIndex(oldInfo);
    if (idx >= 0) {
      beginRemoveRows(QModelIndex(), idx, idx);
      m_contacts.remove(idx);
      endRemoveRows();
    }
  }
}

Qt::ItemFlags ContactsModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

//////////////////////////////////////////////////////////////////////

class ContactItem: public QObject {

};

#if 0
class ContactItem:public QTreeWidgetItem {
public:
  ContactItem(QTreeWidget * parent, const MechInfo & mi, double dist, double bearing):
    QTreeWidgetItem(parent), bgColor(255,255,255), distance(dist)
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
		  QString("s: %1 h: %2�").arg(mi.getSpeed(),3,'f',0)
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
    setText(5, QString("s: %1 h: %2�").arg(mi.getSpeed(),3,'f',0)
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

    if (text(1).toUpper()==text(1))
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
#endif

//////////////////////////////////////////////////////////////////////
BattleContactWidget::BattleContactWidget(QWidget *parent,
                                         const char *name,
                                         const QStringList &args) :
        QWidget(parent), core(BattleCore::getInstance())
{
  setObjectName(name);

  connect(core, SIGNAL(queueMechInfoChange(const MechInfo&, const MechInfo&)),
          this, SLOT(slotUpdateMechInfo(const MechInfo&, const MechInfo&)));

  //  connect(core, SIGNAL(flushDisplayChange()),
  //	  this, SLOT(updateList()));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setContentsMargins(0,0,0,0);
  //l->setAutoAdd(true);
  BattleHeatWidget *bh = new BattleHeatWidget(this, "HeatBar", QStringList());
  l->addWidget(bh);
  bh->setFixedHeight(15);
  listView = new QTreeView(this);
  l->addWidget(listView);
  con_model = new ContactsModel(this);

  //  listView->setPalette(QPalette(QColor(0,0,0),QColor(200,200,200)));
  listView->setRootIsDecorated(false);
  QPalette pal = listView->palette();
  pal.setColor(QPalette::Button, QColor(0,0,0));
  pal.setColor(QPalette::Background, QColor(200,200,200));
  listView->setPalette(pal);

  listView->setFocusPolicy(Qt::NoFocus);
  listView->viewport()->setFocusPolicy(Qt::NoFocus);

  listView->setModel(con_model);

  listView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(popmeup(const QPoint &)));

  //TODO: add eventFilter to catch mouse press

  //connect(listView, SIGNAL(rightButtonPressed(QListViewItem * ,const QPoint &, int)),
  //        this, SLOT(popmeup(QListViewItem * ,const QPoint &, int)));

#if 0
  listView->addColumn(""); // SE 0
  listView->addColumn(""); // ID 1
  listView->addColumn("Name"); // 2
  listView->addColumn("Pos"); // 3
  listView->addColumn("Range"); // 4
  listView->addColumn("Movement"); // 5
  listView->addColumn(""); // Status 6
  listView->addColumn("Age"); // 7

  listView->setColumnAlignment(1, AlignHCenter);
  listView->setColumnAlignment(4, AlignHCenter);
  listView->setColumnAlignment(5, AlignHCenter);
  listView->setColumnAlignment(6, AlignHCenter);
  listView->setColumnAlignment(7, AlignRight);
  listView->setAllColumnsShowFocus(true);
  listView->setSelectionMode(QListView::NoSelection);


#endif

  popup = new QMenu(this);
  QAction *ta;
  ta = popup->addAction(i18n("&Lock"), this, SLOT(slotPopup()));
  ta->setProperty("id", 1);
  ta = popup->addAction(i18n("&Scan"), this, SLOT(slotPopup()));
  ta->setProperty("id", 2);
  ta = popup->addAction(i18n("&Report"), this, SLOT(slotPopup()));
  ta->setProperty("id", 3);
  ta = popup->addAction(i18n("&Face towards"), this, SLOT(slotPopup()));
  ta->setProperty("id", 4);
  ta = popup->addAction(i18n("R&emove"), this, SLOT(slotPopup()));
  ta->setProperty("id", 5);
  ta = popup->addSeparator();
  ta = popup->addAction(i18n("&Always show"), this, SLOT(slotPopup()));
  ta->setProperty("id", 6);
  ta = popup->addAction(i18n("Always &hide"), this, SLOT(slotPopup()));
  ta->setProperty("id", 7);
  ta = popup->addAction(i18n("Auto&matic"), this, SLOT(slotPopup()));
  ta->setProperty("id", 8);

  setFocusPolicy(Qt::NoFocus);
}

BattleContactWidget::~BattleContactWidget() {
  BattleCore::returnInstance();
}

void BattleContactWidget::popmeup(const QPoint &point) {
  QModelIndex idx = listView->indexAt(point);
  if (!idx.isValid()) return;

  idx = con_model->index(idx.row(), 1, QModelIndex());

  currentID=con_model->data(idx, Qt::DisplayRole).toString();

  if (currentID.isEmpty()) return;

  popup->exec(listView->mapToGlobal(point));
}

#if 0
void BattleContactWidget::popmeup(QTreeViewItem *li, const QPoint &p, int) {
  if (li) {
    currentID=li->text(1);
    popup->exec(p);
  }
}
#endif

void  BattleContactWidget::slotPopup() {
  QAction *ta = dynamic_cast<QAction*>(sender());
  if (ta==NULL) return;

  int fun = ta->property("id").toInt();

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

  con_model->updateMechInfo(oldInfo, mechInfo);

#if 0
  if((oldInfo.getId() == core->getOwnId()) || (mechInfo.getId() == core->getOwnId())) {

    if(oldInfo.isValid() && items.find(oldInfo.getId().toUpper()) != items.end()) {
      delete items[oldInfo.getId().toUpper()];
      items.erase(oldInfo.getId().toUpper());
    }

    if(mechInfo.isValid() && items.find(mechInfo.getId().toUpper()) != items.end()) {
      delete items[mechInfo.getId().toUpper()];
      items.erase(mechInfo.getId().toUpper());
    }

    return;
  }

  if (mechInfo.isValid()) {
    MechInfo self = core->getMechInfo(core->getOwnId());
    SubPos dist = NormHexLayout::difference(self.getPos(),mechInfo.getPos());
    if(items.find(mechInfo.getId().toUpper()) != items.end()) {
      items[mechInfo.getId().toUpper()]->setInfo(mechInfo, dist.getDistance(),
                                                 dist.getAngleDeg());
    } else {
      ContactItem * i=new ContactItem(listView,mechInfo,dist.getDistance(),dist.getAngleDeg());
      items[mechInfo.getId().toUpper()] = i;
    }
    listView->sort();
  } else {
    if(items.find(oldInfo.getId().toUpper()) != items.end()) {
      delete items[oldInfo.getId().toUpper()];
      items.erase(oldInfo.getId().toUpper());
    }
    // sorting info isn't changed...
  }
#endif
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
