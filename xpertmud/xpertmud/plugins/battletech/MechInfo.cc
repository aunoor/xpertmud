#include "MechInfo.h"
#include "BattleCore.h"
#include <qstringlist.h>

MechInfo::MechInfo():
  valid(false), los(false), name("something"), pHasPos(false),
  pHasTurret(false), pHasHeading(false), pHasArc(false),
  pHasSensors(false), pHasType(false), pHasSpeed(false),
  pHasVSpeed(false), pHasJumpHeading(false), pHasWeight(false),
  pHasHeat(false), pHasHeatDissipation(false), pHasStatus(false),
  pHasMaxRunningSpeed(false), pHasMaxWalkingSpeed(false),
  pHasMaxBackingSpeed(false), pHasDesiredSpeed(false),
  pHasVisibleOverride(false), pHasOriginalArmor(false),
  pHasCurrentArmor(false)
{}

/**
 * Don't call with a mechinfo from a different mech,
 * this won't be checked explicitely!
 */
void MechInfo::update(const MechInfo &u) {
  if(u.isValid()) {
    // don't compare with u.id.lower(), so FoF info won't be disrupted with seismic updates
    if(!valid || id.lower() == u.id) {
      setId(u.getId());
    }
  }
  if(u.isInLOS()) {
    los = true;
    lastContact = u.lastContact;
  }

  if(u.hasName()) { setName(u.getName()); }
  if(u.hasPos()) { setPos(u.getPos()); }
  if(u.hasTurretOffset()) { setTurretOffset(u.getTurretOffset()); }
  if(u.hasHeading()) { setHeading(u.getHeading()); }
  if(u.hasArc()) { setArc(u.getArc()); }
  if(u.hasSensorHits()) { setSensorHits(u.getSensorHits()); }
  if(u.hasType()) { setType(u.getType()); }
  if(u.hasSpeed()) { setSpeed(u.getSpeed()); }
  if(u.hasVSpeed()) { setVSpeed(u.getVSpeed()); }
  if(u.hasJumpHeading()) { setJumpHeading(u.getJumpHeading()); }
  if(u.hasWeight()) { setWeight(u.getWeight()); }
  if(u.hasHeat()) { setHeat(u.getHeat()); }
  if(u.hasHeatDissipation()) { setHeatDissipation(u.getHeatDissipation()); }
  if(u.hasStatus()) { setStatus(u.getStatus()); }
  if(u.hasRunningSpeed()) { setRunningSpeed(u.getRunningSpeed()); }
  if(u.hasWalkingSpeed()) { setWalkingSpeed(u.getWalkingSpeed()); }
  if(u.hasBackingSpeed()) { setBackingSpeed(u.getBackingSpeed()); }
  if(u.hasDesiredSpeed()) { setDesiredSpeed(u.getDesiredSpeed()); }
  if(u.hasOriginalArmor()) { setOriginalArmor(u.getOriginalArmor()); }
  if(u.hasCurrentArmor()) { setCurrentArmor(u.getCurrentArmor()); }

}

QString MechInfo::scanStatus() const {
  QString buf;
  if(sensors & PRIMARY_SENSOR)
    buf += 'P';
  if(sensors & SECONDARY_SENSOR)
    buf += 'S';
  if(arc == UNDEF_ARC)
    buf='-';
  else {
    if((arc & FORWARD_ARC) != 0)
      buf += '*';
    if((arc & LEFT_ARC) != 0)
      buf += 'l';
    if((arc & RIGHT_ARC) != 0)
      buf += 'r';
    if((arc & REAR_ARC) != 0)
      buf += 'v';
    if((arc & TURRET_ARC) != 0)
      buf += '*';
  }

  return buf;
}

QString MechInfo::toString(const MechInfo &me) const {
  QString ret;
  QString buf;
  QTextOStream os(&ret);
  os << id << '|';
  if(arc == UNDEF_ARC)
    buf='-';
  else {
    if((arc & FORWARD_ARC)!= 0)
      buf += '*';
    if((arc & LEFT_ARC) != 0)
      buf += 'l';
    if((arc & RIGHT_ARC) != 0)
      buf += 'r';
    if((arc & REAR_ARC) != 0)
      buf += 'v';
    if((arc & TURRET_ARC) != 0)
      buf += '*';
  }
  os << buf << '|';
  buf="";
  if(sensors & PRIMARY_SENSOR)
    buf += 'P';
  if(sensors & SECONDARY_SENSOR)
    buf += 'S';
  os << buf << '|'
     << ((char)(type)) << '|' 
     << name << '|' 
     << pos.getX() << '|'  
     << pos.getY() << '|'  
     << pos.getZ() << '|';
  
  SubPos dist = NormHexLayout::difference(me.pos,pos);
  os << QString::number(me.pos.calcDistance(pos),'f',3) << '|'
     << (int)(dist.getAngleDeg()) << '|'
     << QString::number(speed,'f',3) << '|' // Speed
     << QString::number(v_speed,'f',3) << '|' // V-Speed
     << (int)(heading*180.0/M_PI) << '|';
  if(status.contains('J'))
    os << (int)(jump_heading*180.0/M_PI) << '|';
  else
    os << '-' << '|';
  os << QString::number((pos.getDistance()),'f',3) << '|'
     << (int)(pos.getAngleDeg()) << '|'
     << weight << '|' // Weight in tons
     << heat << '|' // Heat
     << status << '|' // Status code
     << age();
  return ret;
}

bool MechInfo::isCliff(const MapTile &tile1,const MapTile &tile2) const {
  int h1, h2;
  switch(type) {
  case 'B': // Biped
  case 'Q': // Quad
    if (status.contains("J")) { // Jumping mech, cliffs similar to vtol (without wood exception)
      // TODO: need to check if water counts as Z0 in a jump :)
      h1 = tile1.getRealHeight();
      h2 = tile2.getRealHeight();
      if((h1 > pos.getZ()) ^ (h2 > pos.getZ()))
	return true;
      return false;


    } else {
      if(abs(tile1.getRealHeight() - tile2.getRealHeight()) > 2)
	return true;
      return false;
    }
    break;
  case 'W': // Wheeled
    if(abs(tile1.getRealHeight() - tile2.getRealHeight()) > 1)
      return true;
    if((tile1.getType() ==  tile2.getType() && (tile2.getType() != (char)MapTile::WATER)) ||
	((tile1.getRealHeight() < 0 && tile2.getRealHeight() < 0)))
      return false;
    if(tile1.getType() == (char)MapTile::ROUGH       || tile2.getType() == (char)MapTile::ROUGH       ||
       tile1.getType() == (char)MapTile::LIGHT_WOODS || tile2.getType() == (char)MapTile::LIGHT_WOODS ||
       tile1.getType() == (char)MapTile::FIRE || tile2.getType() == (char)MapTile::FIRE ||
       tile1.getType() == (char)MapTile::HEAVY_WOODS || tile2.getType() == (char)MapTile::HEAVY_WOODS ||
       (tile1.getType() == (char)MapTile::WATER && tile1.getRealHeight() < 0)||
       (tile2.getType() == (char)MapTile::WATER && tile2.getRealHeight() < 0))
      return true;
    return false;
    break;
  case 'T': // Tracked
    if(abs(tile1.getRealHeight() - tile2.getRealHeight()) > 1)
      return true;
    if((tile1.getType() ==  tile2.getType() && (tile2.getType() != (char)MapTile::WATER)) ||
	((tile1.getRealHeight() < 0 && tile2.getRealHeight() < 0)))
      return false;    
    if(tile1.getType() == (char)MapTile::HEAVY_WOODS || tile2.getType() == (char)MapTile::HEAVY_WOODS ||
       tile1.getType() == (char)MapTile::FIRE || tile2.getType() == (char)MapTile::FIRE ||
       (tile1.getType() == (char)MapTile::WATER && tile1.getRealHeight() < 0)||
       (tile2.getType() == (char)MapTile::WATER && tile2.getRealHeight() < 0))
      return true;
    return false;
    break;
  case 'H': // Hoover
    if(tile1.getType() == (char)MapTile::WATER) h1 = 0; else h1 = tile1.getRealHeight();
    if(tile2.getType() == (char)MapTile::WATER) h2 = 0; else h2 = tile2.getRealHeight();
    if(abs(h1-h2) > 1)
      return true;
    if( tile1.getType() ==  tile2.getType())
      return false;    
    if(tile1.getType() == (char)MapTile::HEAVY_WOODS || tile2.getType() == (char)MapTile::HEAVY_WOODS ||
       tile1.getType() == (char)MapTile::FIRE || tile2.getType() == (char)MapTile::FIRE ||
       tile1.getType() == (char)MapTile::LIGHT_WOODS || tile2.getType() == (char)MapTile::LIGHT_WOODS)
      return true;
    return false;
    break;
  case 'V': // VTOL
  case 'F': // Fighter
  case 'A': // Aero Dropship
  case 'D': // Dropship
    if(tile1.getType() == (char)MapTile::WATER) h1 = 0; else h1 = tile1.getRealHeight();
    if(tile2.getType() == (char)MapTile::WATER) h2 = 0; else h2 = tile2.getRealHeight();
    if(tile1.getType() == (char)MapTile::HEAVY_WOODS) h1++;
    if(tile1.getType() == (char)MapTile::LIGHT_WOODS) h1++;
    if(tile2.getType() == (char)MapTile::HEAVY_WOODS) h2++;
    if(tile2.getType() == (char)MapTile::LIGHT_WOODS) h2++;
    if((h1 > pos.getZ()) ^ (h2 > pos.getZ()))
      return true;
    return false;
  case 'N': // Naval Surface displacement Vehicle
  case 'Y': // Hydrofoil
  case 'U': // Submarine TODO: FIXME: underwater cliffs like vtol
    if(tile1.getType() != (char)MapTile::WATER || tile2.getType() != (char)MapTile::WATER)
      return true;
    if((tile1.getRealHeight() > pos.getZ()) ^ (tile2.getRealHeight() > pos.getZ()))
      return true;
    return false;
    break;
  case 'S': // BattleSuit
    //TODO
  case 'I': // Infantry
    //TODO
  default:
    return false;
  }
}

MechInfo::UnitType MechInfo::normUnitType(QChar c) {
  switch((MechInfo::UnitType)((int)c)) {
  case MechInfo::UNIT_BIPED:
  case MechInfo::UNIT_QUAD:
  case MechInfo::UNIT_HOVER:
  case MechInfo::UNIT_TRACKED:
  case MechInfo::UNIT_WHEELED:
  case MechInfo::UNIT_SDISP:
  case MechInfo::UNIT_HYDROF:
  case MechInfo::UNIT_SUB:
  case MechInfo::UNIT_VTOL:
  case MechInfo::UNIT_AERO:
  case MechInfo::UNIT_DS_AREO:
  case MechInfo::UNIT_DS_SPHERE:
  case MechInfo::UNIT_BATTLESUIT:
  case MechInfo::UNIT_INFANTRY:
  case MechInfo::UNIT_INSTALLATION:
     return (MechInfo::UnitType)((int)c);
    break;
  default:
    return MechInfo::UNIT_UNKNOWN;
  }
}
