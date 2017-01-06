#include "BattleWeapons.h"
#include "BattleCore.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>
#include <QHeaderView>

#include <klocale.h>
#include <cassert>

//////////////////////////////////////////////////////////////////////

WeaponsModel::WeaponsModel(QObject *parent) : QAbstractItemModel(parent) {
  core = BattleCore::getInstance();
}

int WeaponsModel::rowCount(const QModelIndex &parent) const {
  return m_weapons.count();
}

int WeaponsModel::columnCount(const QModelIndex &parent) const {
  return 9;
}

QModelIndex WeaponsModel::index(int row, int column, const QModelIndex &parent) const {
  if (row>=m_weapons.count()) return QModelIndex();
  if (column>=columnCount(QModelIndex())) return QModelIndex();
  if (parent.isValid()) return QModelIndex();
  return createIndex(row, column);
}

QModelIndex WeaponsModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

QVariant WeaponsModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case 0: // 0  Nr
        case 1: // 1  Section
          return QString("");
          break;
        case 2: //Name
          return QString("Name");
        case 3: // 3, was BTH, remove sometimes
        case 4: // 4 Status
        case 5: // 5 Ammo Type
          return QString("");
        case 6: //weapon heat
          return QString("H");
        case 7: //Ranges
          return QString("Ranges");
        case 8: //damage
          return QString("D");
        default:
          return QVariant();
      }
    }//if DisplayRole

    if (role == Qt::TextAlignmentRole) {
      return Qt::AlignHCenter;
    }//if TextAlignment

  }//if horizontal

  return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags WeaponsModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QVariant WeaponsModel::data(const QModelIndex &index, int role) const {

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

  const WeaponInfo *wi = &(m_weapons.at(index.row()));
  const WeaponStat ws=core->getWeaponStat(wi->getId());

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0:
        return wi->getId();
      case 1:
        return wi->getSection();
      case 2:
        if(wi->hasTechName())
          return wi->getTechName() + "." + wi->getName();
        else
          return wi->getName();
      case 3:
/*
    if(wi.hasBth()) {
      setText(3, QString::number(wi.getBth()));
    } else {
      setText(3, "n/a");
    }
*/
        //TODO: there must be BTH?
        return QVariant();
      case 4: if(wi->isRecycling())
          return QString::number(wi->getRecycleTime());
         else
          return wi->getStatus();
      case 5:
          return wi->getAmmoType();
      case 6:
        if (ws.hasHeat())
          return QString::number(double(ws.getHeat()) / 10.0);
        else
          return QString("n/a");
      case 7:
        if(ws.hasMinRange() && ws.hasShortRange() &&
           ws.hasMidRange() && ws.hasLongRange()) {
          QString ranges;
          QTextStream str(&ranges, QIODevice::WriteOnly);
          str << ws.getMinRange() << " "
              << ws.getShortRange() << " "
              << ws.getMidRange() << " "
              << ws.getLongRange();
          return ranges;
        } else {
          return QString("n/a");
        }
      case 8:
        if(ws.hasDamage())
          return QString::number(ws.getDamage());
         else
          return QString("n/a");

      default:
        QVariant();
    }
  } //if DisplayRole

  if (role == Qt::BackgroundRole) {
    QColor heatColor = QColor(0,0,0);

    if(ws.hasHeat()) {
      int newHeat = heat + ws.getHeat();
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
    }

    return heatColor;
  }//BackgroundRole

  if (role == Qt::TextColorRole) {
    QColor textColor = QColor(Qt::lightGray);
    if(wi->isRecycling()) textColor = QColor(Qt::gray);
    return textColor;
  }

  return QVariant();
}

void WeaponsModel::updateWeaponInfo(int id) {
  WeaponInfo wi=core->getWeaponInfo(id);
  WeaponStat ws=core->getWeaponStat(wi.key());
  if(m_weapons.size() <= id) {
    beginInsertRows(QModelIndex(), id + 1, id + 1);
    m_weapons.resize(id + 1);
    endInsertRows();
  }
  m_weapons[id] = wi;
}

void WeaponsModel::changeNrWeapons(int count) {
  if (m_weapons.count()==count) return;
  beginResetModel();
  m_weapons.resize(count);
  endResetModel();
}

void WeaponsModel::setHeat(int heat) {
  this->heat = heat;
}

void WeaponsModel::setHeatDissipation(int heatDissipation) {
  this->heatDissipation = heatDissipation;
}

//////////////////////////////////////////////////////////////////////

WeaponView::WeaponView(QWidget* parent, const char* name, const QStringList& /*args*/):
  QWidget(parent), core(BattleCore::getInstance())
{
  setObjectName(name);
  connect(core, SIGNAL(weaponChange(int)), this, SLOT(slotUpdateEntry(int)));
  connect(core, SIGNAL(nrWeaponsChange(int)), this, SLOT(slotNrWeaponsChange(int)));
  connect(core,SIGNAL(heatChanged(int)), this, SLOT(heatChanged(int)));
  connect(core,SIGNAL(heatDissipationChanged(int)), this, SLOT(heatDissipationChanged(int)));
  QVBoxLayout *l = new QVBoxLayout(this);
  l->setContentsMargins(0,0,0,0);
  listView=new QTreeView(this);
  l->addWidget(listView);

  listView->setRootIsDecorated(false);
  listView->setStyleSheet("QTreeView {background: black;}");

  weapons_model = new WeaponsModel(listView);
  listView->setModel(weapons_model);

  listView->header()->resizeSection(0, listView->fontMetrics().maxWidth()*2); //id
  listView->header()->resizeSection(1, listView->fontMetrics().maxWidth()*3); //section
  listView->header()->resizeSection(2, listView->fontMetrics().maxWidth()*11); //name
  listView->header()->resizeSection(3, listView->fontMetrics().maxWidth()*4); //bth
  listView->header()->resizeSection(4, listView->fontMetrics().maxWidth()*3); //status
  listView->header()->resizeSection(5, listView->fontMetrics().maxWidth()*2); //Ammo type
  listView->header()->resizeSection(6, listView->fontMetrics().maxWidth()*4); //heat
  listView->header()->resizeSection(7, listView->fontMetrics().maxWidth()*6); //ranges
  listView->header()->resizeSection(8, listView->fontMetrics().maxWidth()*3); //damage
}

WeaponView::~WeaponView() {
  BattleCore::returnInstance();
}

void WeaponView::slotFunctionCall(int /*func*/, const QVariant & /*args*/, QVariant & /*result*/) {
}

void WeaponView::slotUpdateEntry(int id) {
  weapons_model->updateWeaponInfo(id);
}

void WeaponView::slotNrWeaponsChange(int nr) {
  weapons_model->changeNrWeapons(nr);
}

void WeaponView::heatChanged(int nheat) {
  weapons_model->setHeat(nheat);
  updateEntries();
}

void WeaponView::heatDissipationChanged(int nheatDissipation) {
  weapons_model->setHeatDissipation(nheatDissipation);
  updateEntries();
}

void WeaponView::updateEntries() {
  for(int i=0; i<weapons_model->rowCount(QModelIndex()); ++i) {
    slotUpdateEntry(i);
  }
}
