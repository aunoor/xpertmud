// -*- c++ -*-
#ifndef MECHINFO_H
#define MECHINFO_H

#include "Positions.h"
//#include "BattleCore.h"
#include <qstring.h>
#include <qdatetime.h>
#include <qmap.h>

class QStringList;
//////////////////////////////////////////////////////////////////////
class MapTile;

class MechInfo {
public:
  enum UnitType {
    UNIT_UNKNOWN='-',
    UNIT_BIPED='B',
    UNIT_QUAD='Q',
    UNIT_HOVER='H',
    UNIT_TRACKED='T',
    UNIT_WHEELED='W',
    UNIT_SDISP='N',
    UNIT_HYDROF='Y',
    UNIT_SUB='U',
    UNIT_VTOL='V',
    UNIT_AERO='F',
    UNIT_DS_AREO='A',
    UNIT_DS_SPHERE='D',
    UNIT_BATTLESUIT='S',
    UNIT_INFANTRY='I',
    UNIT_INSTALLATION='i'
  };
  enum Arc {
    UNDEF_ARC=0,
    FORWARD_ARC=1,
    LEFT_ARC=2,
    RIGHT_ARC=4,
    REAR_ARC=8,
    TURRET_ARC=16
  };
  enum SensorHit {
    NO_SENSOR=0,
    PRIMARY_SENSOR=1,
    SECONDARY_SENSOR=2
  };

  /**
   * Construct an empty (e.g. invalid) MechInfo object
   * if you set an id, the object becomes valid
   * you can use the set functions to fill it with values
   * or use one of the convenience constructors for special
   * purposes.
   */
  MechInfo();

  void update(const MechInfo &u);

  const QString & getId() const { return id; }
  void setId(const QString& id) { 
    this->id = id;
    valid = true;
  }
  bool isValid() const { return valid; }

  int age() const {
    if (los)
      return 0;
    return lastContact.secsTo(QDateTime::currentDateTime());
  }
  void setInLOS(bool l) { 
    if (los && !l) {
      lastContact=QDateTime::currentDateTime();
      sensors = NO_SENSOR;
    }
    los=l;
  }
  bool isInLOS() const { return los; }

  bool hasVisibleOverride() const { return pHasVisibleOverride; }
  void setVisibleOverride(bool override, bool vis) {
    pHasVisibleOverride=override;
    if (override) {
      visibleOverride=vis;
    }
  }

  bool visible() const {
    if (hasVisibleOverride()) {
      return visibleOverride;
    } else {
      return age() < 60;
    }
  }


  const QString & getName() const { return name; }
  void setName(const QString& name) {
    this->name = name;
  }
  bool hasName() const { return name != "something"; }

  const SubHEXPos & getPos() const { return pos; }
  void setPos(const SubHEXPos& pos) {
    this->pos = pos;
    pHasPos = true;
  }
  bool hasPos() const { return pHasPos; }

  double getTurretOffset() const { return turret; }
  void setTurretOffset(double turret) {
    this->turret = turret;
    pHasTurret = true;
  }
  bool hasTurretOffset() const { return pHasTurret; }

  /** Heading in rad. */
  double getHeading() const { return heading; }
  double getHeadingDeg() const { return heading*180.0/M_PI; }
  void setHeading(double heading) {
    this->heading = heading;
    pHasHeading = true;
  }
  bool hasHeading() const { return pHasHeading; }

  Arc getArc() const { return arc; }
  void setArc(Arc arc) {
    this->arc = arc;
    pHasArc = true;
  }
  bool hasArc() const { return pHasArc; }

  SensorHit getSensorHits() const { return sensors; }
  void setSensorHits(SensorHit sensors) {
    this->sensors = sensors;
    pHasSensors = true;
  }
  bool hasSensorHits() const { return pHasSensors; }

  char getType() const { return (char)type; }
  void setType(QChar type) {
    this->type = normUnitType(type);
    pHasType = true;
  }
  bool hasType() const { return pHasType; }

  double getSpeed() const { return speed; }
  void setSpeed(double speed) {
    this->speed = speed;
    pHasSpeed = true;
  }
  bool hasSpeed() const { return pHasSpeed; }

  double getVSpeed() const { return v_speed; }
  void setVSpeed(double vspeed) {
    this->v_speed = vspeed;
    pHasVSpeed = true;
  }
  bool hasVSpeed() const { return pHasVSpeed; }

  /** Heading in rad. */
  double getJumpHeading() const { return jump_heading; }
  void setJumpHeading(double heading) {
    this->jump_heading = heading;
    pHasJumpHeading = true;
  }
  bool hasJumpHeading() const { return pHasJumpHeading; }

  /** Heading in rad. */
  int getWeight() const { return weight; }
  void setWeight(int weight) {
    this->weight = weight;
    pHasWeight = true;
  }
  bool hasWeight() const { return pHasWeight; }

  int getHeat() const { return heat; }
  void setHeat(int heat) {
    this->heat = heat;
    pHasHeat = true;
  }
  bool hasHeat() const { return pHasHeat; }

  int getHeatDissipation () const { return heatDissipation; }
  void setHeatDissipation(int heatDissipation) {
    this->heatDissipation = heatDissipation;
    pHasHeatDissipation = true;
  }
  bool hasHeatDissipation() const { return pHasHeatDissipation; }

  const QString & getStatus() const { return status; }
  void setStatus(const QString& status) {
    this->status = status;
    pHasStatus = true;
  }
  bool hasStatus() const { return pHasStatus; }

  double getRunningSpeed() const { return maxRunningSpeed; }
  void setRunningSpeed(double runningSpeed) {
    this->maxRunningSpeed = runningSpeed;
    pHasMaxRunningSpeed = true;
  }
  bool hasRunningSpeed() const { return pHasMaxRunningSpeed; }

  double getWalkingSpeed() const { return maxWalkingSpeed; }
  void setWalkingSpeed(double walkingSpeed) {
    this->maxWalkingSpeed = walkingSpeed;
    pHasMaxWalkingSpeed = true;
  }
  bool hasWalkingSpeed() const { return pHasMaxWalkingSpeed; }

  double getBackingSpeed() const { return maxBackingSpeed; }
  void setBackingSpeed(double backingSpeed) {
    this->maxBackingSpeed = backingSpeed;
    pHasMaxBackingSpeed = true;
  }
  bool hasBackingSpeed() const { return pHasMaxBackingSpeed; }

  double getDesiredSpeed() const { return desiredSpeed; }
  void setDesiredSpeed(double desiredSpeed) {
    this->desiredSpeed = desiredSpeed;
    pHasDesiredSpeed = true;
  }
  bool hasDesiredSpeed() const { return pHasDesiredSpeed; }

  const QMap<QString,int> & getOriginalArmor() const { return originalArmor; } 
  void setOriginalArmor(const QMap<QString,int> & a) {
    originalArmor=a;
    pHasOriginalArmor=true;
  }
  bool hasOriginalArmor() const { return pHasOriginalArmor; }

  const QMap<QString,int> & getCurrentArmor() const { return currentArmor; } 
  void setCurrentArmor(const QMap<QString,int> & a) {
    currentArmor=a;
    pHasCurrentArmor=true;
  }
  bool hasCurrentArmor() const { return pHasCurrentArmor; }
  /////////////////////////
  // informational stuff

  bool isEnemy() const { return getId().upper() == getId(); };
  bool isCliff(const MapTile &,const MapTile &) const;
  QString scanStatus() const;

private:
  // every valid MechInfo has an id
  QString id; 
  bool valid; 

  // LOS stuff, used to identify outdated contacts
  QDateTime lastContact;
  bool los;
  
  QString name;

  SubHEXPos pos;
  bool pHasPos;

  double turret;
  bool pHasTurret;

  double heading;
  bool pHasHeading;

  Arc arc;
  bool pHasArc;

  SensorHit sensors;
  bool pHasSensors;

  UnitType type;
  bool pHasType;

  double speed;
  bool pHasSpeed;

  double v_speed;
  bool pHasVSpeed;

  double jump_heading;
  bool pHasJumpHeading;

  int weight;
  bool pHasWeight;

  int heat;
  bool pHasHeat;

  int heatDissipation;
  bool pHasHeatDissipation;

  QString status;
  bool pHasStatus;

  double maxRunningSpeed;
  bool pHasMaxRunningSpeed;

  double maxWalkingSpeed;
  bool pHasMaxWalkingSpeed;

  double maxBackingSpeed;
  bool pHasMaxBackingSpeed;

  double desiredSpeed;
  bool pHasDesiredSpeed;

  bool visibleOverride;
  bool pHasVisibleOverride;


  QMap<QString,int> originalArmor;
  bool pHasOriginalArmor;

  QMap<QString,int> currentArmor;
  bool pHasCurrentArmor;

public:
  /*
   response:
        ID|AC|SE|UT|MN|X|Y|Z|RN|BR|SP|VS|HD|JH|RTC|BTC|TN|HT|FL|AG

   0 ID: mechid, ID of the unit
   1 AC: arc, weapon arc the unit is in
   2 SE: sensors, sensors that see the unit
   3 UT: unit type character
   4 MN: string, mechname of unit
   5 X,6 Y,7 Z: coordinates of unit
   8 RN: range, range to unit
   9 BR: degree, bearing to unit
  10 SP: speed, speed of unit
  11 VS: speed, vertical speed of unit
  12 HD: degree, heading of unit
  13 JH: degree, jump heading, or '-' if not jumping
  14 RTC: range, range from unit to X,Y center
  15 BTC: degree, bearing from unit to X,Y center
  16 TN: integer, unit weight in tons
  17 HT: heatmeasure, unit's apparent heat (overheat)
  18 FL: unit status string
  19 AG: Age in secs since last LOS contact (0 == LOS maintained)
  */
  QString toString(const MechInfo &me) const;

private:
  static UnitType normUnitType(QChar c);
};



#endif // #ifndef MECHINFO_H
