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
BattleContactWidget::BattleContactWidget(QWidget *parent,
                                         const char *name,
                                         const QStringList &args) :
        QWidget(parent), core(BattleCore::getInstance())
{
  setObjectName(name);

  connect(core, SIGNAL(queueMechInfoChange(const MechInfo&, const MechInfo&)),
          this, SLOT(slotUpdateMechInfo(const MechInfo&, const MechInfo&)));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setContentsMargins(0,0,0,0);
  BattleHeatWidget *bh = new BattleHeatWidget(this, "HeatBar", QStringList());
  l->addWidget(bh);
  bh->setFixedHeight(15);
  listView = new QTreeView(this);
  l->addWidget(listView);
  con_model = new ContactsModel(this);

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

  listView->header()->resizeSection(0, listView->fontMetrics().maxWidth()*4);
  listView->header()->resizeSection(1, listView->fontMetrics().maxWidth()*3);
  listView->header()->resizeSection(6, listView->fontMetrics().maxWidth()*2);
  listView->header()->resizeSection(7, listView->fontMetrics().maxWidth()*7);

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

void BattleContactWidget::slotPopup() {
  QAction *ta = dynamic_cast<QAction *>(sender());
  if (ta == NULL) return;

  int fun = ta->property("id").toInt();

  switch (fun) {
    case 1:
      core->slotSend("lock " + currentID + "\n");
      break;
    case 2:
      core->slotSend("scan " + currentID + "\n");
      break;
    case 3: {
      QVariant result;
      emit callback(0, currentID, result);
    }
      break;
    case 4: {
      MechInfo self = core->getMechInfo(core->getOwnId());
      MechInfo other = core->getMechInfo(currentID);
      SubPos dist = NormHexLayout::difference(self.getPos(), other.getPos());
      core->slotSend(QString("heading %1\n").arg(dist.getAngleDeg(), 3, 'f', 0));
    }
      break;
    case 5:
      core->deleteMechInfo(currentID);
      break;
    case 6: // always show
      core->setVisibleOverride(currentID, true, true);
      break;
    case 7: //always hide
      core->setVisibleOverride(currentID, true, false);
      break;
    case 8: //automagic
      core->setVisibleOverride(currentID, false, true);
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
}