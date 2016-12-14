#include "WeaponInfo.h"

WeaponStat::WeaponStat():
  valid(false), pHasName(false), pHasMinRange(false),
  pHasShortRange(false), pHasMidRange(false), pHasLongRange(false),
  pHasHeat(false), pHasDamage(false), pHasTechName(false)
{}

void WeaponStat::update(const WeaponStat& stat) {
  if(!isValid() && stat.isValid()) { setId(stat.getId()); }
  if(stat.hasName()) { setName(stat.getName()); }
  if(stat.hasMinRange()) { setMinRange(stat.getMinRange()); }
  if(stat.hasShortRange()) { setShortRange(stat.getShortRange()); }
  if(stat.hasMidRange()) { setMidRange(stat.getMidRange()); }
  if(stat.hasLongRange()) { setLongRange(stat.getLongRange()); }
  if(stat.hasHeat()) { setHeat(stat.getHeat()); }
  if(stat.hasDamage()) { setDamage(stat.getDamage()); }
  if(stat.hasTechName()) { setTechName(stat.getTechName()); }
}


WeaponInfo::WeaponInfo():
  valid(false), pHasName(false), pHasStatus(false),
  pHasSection(false), pHasBth(false), pHasAmmoType(false),
  recycling(false), pHasTechName(false)
{}

void WeaponInfo::update(const WeaponInfo& info) {
  if(!isValid() && info.isValid()) { setId(info.getId()); }
  if(info.hasName()) { setName(info.getName()); }

  if(info.hasStatus()) { setStatus(info.getStatus()); }
  else { pHasStatus = false; }

  if(info.hasSection()) { setSection(info.getSection()); }
  if(info.hasBth()) { setBth(info.getBth()); }
  if(info.hasAmmoType()) { setAmmoType(info.getAmmoType()); }

  if(info.isRecycling()) { setRecycleTime(info.getRecycleTime()); }
  else { recycling = false; }

  if(info.hasTechName()) { setTechName(info.getTechName()); }
}
