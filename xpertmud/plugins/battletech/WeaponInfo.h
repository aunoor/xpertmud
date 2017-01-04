// -*- c++ -*-
#ifndef WEAPONINFO_H
#define WEAPONINFO_H

#include <qstring.h>
#include <map>
#include <vector>


/**
 * This one has a simple name and a tech string.
 */
class WeaponStat {
public:
  WeaponStat();
  /**
   * Update from another WeaponStat. The id's are
   * _not_ checked, so be careful.
   */
  void update(const WeaponStat& stat);

  int getId() const { return id; }
  void setId(int id) {
    this->id = id;
    valid = true;
  }
  bool isValid() const { return valid; }

  QString getName() const { return name; }
  void setName(const QString& name) {
    this->name = name;
    pHasName = true;
  }
  bool hasName() const { return pHasName; }

  int getMinRange() const { return minRange; }
  void setMinRange(int minRange) {
    this->minRange = minRange;
    pHasMinRange = true;
  }
  bool hasMinRange() const { return pHasMinRange; }

  int getShortRange() const { return shortRange; }
  void setShortRange(int shortRange) {
    this->shortRange = shortRange;
    pHasShortRange = true;
  }
  bool hasShortRange() const { return pHasShortRange; }

  int getMidRange() const { return midRange; }
  void setMidRange(int midRange) {
    this->midRange = midRange;
    pHasMidRange = true;
  }
  bool hasMidRange() const { return pHasMidRange; }

  int getLongRange() const { return longRange; }
  void setLongRange(int longRange) {
    this->longRange = longRange;
    pHasLongRange = true;
  }
  bool hasLongRange() const { return pHasLongRange; }

  // Uses REAL heat, not heat/10!!!
  int getHeat() const { return heat; }
  void setHeat(int heat) {
    this->heat = heat;
    pHasHeat = true;
  }
  bool hasHeat() const { return pHasHeat; }

  int getDamage() const { return damage; }
  void setDamage(int damage) {
    this->damage = damage;
    pHasDamage = true;
  }
  bool hasDamage() const { return pHasDamage; }

  QString getTechName() const { return techName; }
  void setTechName(const QString& techName) {
    this->techName = techName;
    pHasTechName = true;
  }
  bool hasTechName() const { return pHasTechName; }

  QString key() const {
    if(hasTechName()) {
      return getTechName() + "." + getName();
    } else {
      return QString("??.") + getName();
    }
  }
private:
  int id;
  bool valid;

  QString name;
  bool pHasName;

  int minRange;
  bool pHasMinRange;

  int shortRange;
  bool pHasShortRange;

  int midRange;
  bool pHasMidRange;

  int longRange;
  bool pHasLongRange;

  int heat;
  bool pHasHeat;

  int damage;
  bool pHasDamage;

  QString techName;
  bool pHasTechName;
};

/**
 * A class which handles the weapon stats.
 * Gets its info during .ws parsing or from
 * a database (TODO) or from hudinfo wl
 */
class WeaponStats {
  typedef std::map<QString, int> statMapT;
public:
  WeaponStats() {}
  WeaponStat getStat(const QString& key) {
    statMapT::iterator it = statsMap.find(key);
    if(it != statsMap.end())
      return stats[(*it).second];
    return WeaponStat();
  }

  WeaponStat getStat(unsigned int id) {
    if(stats.size() > id)
      return stats[id];
    return WeaponStat();
  }

  void addStat(WeaponStat& stat) {
    if(!stat.isValid())
      stat.setId(stats.size());
    if((int)stats.size() <= stat.getId())
      stats.resize(stat.getId()+1);

    stats[stat.getId()] = stat;
    statsMap[stat.key()] = stat.getId();
  }

private:
  std::vector<WeaponStat> stats;
  statMapT statsMap;
};

/**
 * This one has a simple name and an extra
 * tech string. So the correct weapon stat
 * object has the name 
 * getTechName()+"."+getName()
 */
class WeaponInfo {
public:
  WeaponInfo();
  void update(const WeaponInfo& info);

  int getId() const { return id; }
  void setId(int id) {
    this->id = id;
    valid = true;
  }
  bool isValid() const { return valid; }

  QString getName() const { return name; }
  void setName(const QString& name) {
    this->name = name;
    pHasName = true;
  }
  bool hasName() const { return pHasName; }

  QString getStatus() const { return status; }
  void setStatus(const QString& status) {
    this->status = status;
    pHasStatus = true;
  }
  bool hasStatus() const { return pHasStatus; }

  QString getSection() const { return section; }
  void setSection(const QString& section) {
    this->section = section;
    pHasSection = true;
  }
  bool hasSection() const { return pHasSection; }

  int getBth() const { return bth; }
  void setBth(int bth) { 
    this->bth = bth;
    pHasBth = true;
  }
  bool hasBth() const { return pHasBth; }

  QString getAmmoType() const { return ammoType; }
  void setAmmoType(const QString& ammoType) {
    this->ammoType = ammoType;
    pHasAmmoType = true;
  }
  bool hasAmmoType() const { return pHasAmmoType; }

  int getRecycleTime() const { return recycleTime; }
  void setRecycleTime(unsigned int time) {
    recycleTime = time;
    recycling = true;
  }
  bool isRecycling() const { return recycling; }

  QString getTechName() const { return techName; }
  void setTechName(const QString& techName) {
    this->techName = techName;
    pHasTechName = true;
  }
  bool hasTechName() const { return pHasTechName; }

  QString key() const {
    if(hasTechName()) {
      return getTechName() + "." + getName();
    } else {
      return QString("??.") + getName();
    }
  }
private:
  int id;
  bool valid;

  QString name;
  bool pHasName;

  QString status;
  bool pHasStatus;

  QString section;
  bool pHasSection;

  int bth;
  bool pHasBth;

  QString ammoType;
  bool pHasAmmoType;

  int recycleTime;
  bool recycling;
  
  QString techName;
  bool pHasTechName;
};

#endif
