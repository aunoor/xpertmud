#include "HUDParser.h"

#include <qstringlist.h>
#include <qtextstream.h>
#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#define LAG_PROTECT_TIME 120

void HUDParser::parse(const QString & sn) {
  // first check some special cases
  QString s;
  int pos = sn.findRev('\n');
  if(pos > 0)
    s = sn.left(pos);
  else
    s = sn;
  
  if (s.startsWith("#HUD hudinfo version ")) {
    serverVersion=s.mid(21);
    core->slotLog("Version check: Server is " + serverVersion + " Client is " HUDINFO_VERSION);
  } else if (s.startsWith("#HUD:"+sessionKey+":")) {
    QString cmd=s.mid(6+sessionKey.length());
    if (cmd.startsWith("C:")) {
      parseContacts(cmd.mid(2));
    } else if (cmd.startsWith("T:S# ")) {

      parseTacticalStart(cmd.mid(5));
    } else if (cmd.startsWith("T:L# ")) {
      parseTacticalLine(cmd.mid(5));
    } else if (cmd.startsWith("T:D# ")) {
      // is this flush needed ?
      // would it be enough, when it is flushed
      // when something else changes, like a 
      // hudingo gs event ?
      core->flushDisplay();
      if (tacProtect) {
	tacCount-=LAG_PROTECT_TIME;
	if (tacCount<=0) {
	  tacTime-=tacCount/3-1;
	  core->slotLog(QString("LAG Protect: setting tacTime to %1").arg(tacTime));
	  tacCount=1;
	}
	tacProtect=false;
      }
      // TODO: CALL UPDATE HERE with both tacHEX pos!
      tacLeftUpper=HEXPos(0,0);
      tacRightLower=HEXPos(0,0);
    } else if (cmd.startsWith("GS:R# ")) {
      parseGeneralStatus(cmd.mid(6));
    } else if (cmd.startsWith("SGI:R# ")) {
      parseStaticGeneralInfo(cmd.mid(7));
    } else if (cmd.startsWith("OAS:")) {
      parseOriginalArmorStatus(cmd.mid(4));
    } else if (cmd.startsWith("AS:")) {
      parseArmorStatus(cmd.mid(3));
    } else if (cmd.startsWith("WL:L# ")) {
      parseWeaponListLine(cmd.mid(6));
    } else if (cmd.startsWith("WL:D# ")) {
      hasWeaponList = true;
    } else if (cmd.startsWith("WE:L# ")) {
      //      cout << "Juhaaaaa" << endl;
      parseWeaponStatusLine(cmd.mid(6));
    } else if (cmd.startsWith("WE:D# ")) {
      core->setNrWeapons(maxWeaponId + 1);
      maxWeaponId = -1;
      if (weProtect) {
	weCount-=LAG_PROTECT_TIME;
	if (weCount<=0) {
	  weTime-=weCount/3-1;
	  core->slotLog(QString("LAG Protect: setting weTime to %1").arg(weTime));
	  weCount=1;
	}
	weProtect=false;
      }
    } else {
      core->slotLog("Unrecognized HUDINFO reply ["+cmd+"]");
    }
  } // else simply ignore it

}
/**
   hudinfo gs

   response:
   Exactly once:
	#HUD:<key>:GS:R# ID,X,Y,Z,CH,DH,CS,DS,CH,HD,CF,CV,DV,RC,BC,TU,FL

   ID: mechid, own mech ID
   X, Y, Z : coordinates, current location
   CH: degree, current heading
   DH: degree, desired heading
   CS: speed, current speed,
   DS: speed, desired speed
   CH: heatmeasure, current heat
   HD: heatmeasure, current heat dissipation
   CF: fuel or '-', current fuel or '-' if not applicable)
   CV: speed, current vertical speed
   DV: speed, desired vertical speed
   RC: range, range to center of current hex
   BC: degree, bearing of center of current hex
   TU: offset or '-', torso/turret facing offset (or '-' if not applicable)
   FL: Unit status flags
*/
void HUDParser::parseGeneralStatus(const QString & line) {
  QStringList params=QStringList::split(',',line,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if (params.count()>=14) {
    //    int line=params[0].toInt();
    MechInfo om = core->getMechInfo(params[0]);

    // if no mechinfo there, get the static info of your mech
    if(!om.isValid())
      core->slotSend("hudinfo sgi\n");
    core->setOwnId(params[0].lower());
    // TODO: construct mechinfo for oneself.
    
    MechInfo old_om = om;
    MechInfo update;
    update.setId(params[0].lower());
    update.setInLOS(true);
    double corr = 180.0 + params[14].toDouble();
    if(corr >= 360)
      corr -= 360.0;
    update.setPos(SubHEXPos(params[1].toInt(),params[2].toInt(),
			    params[3].toInt(),corr*M_PI/180.0,
			    params[13].toDouble()));
    update.setHeading(params[4].toDouble()/180.0*M_PI);
    update.setHeat(params[8].toInt());
    update.setSpeed(params[6].toDouble());
    update.setDesiredSpeed(params[7].toDouble());
    update.setHeatDissipation(params[9].toInt()/10);
    update.setVSpeed(params[11].toDouble());
    // glitch in params, you dont get the jump heading of your own
    // mech.
    update.setTurretOffset(params[15].toDouble()/180.0*M_PI);
    update.setStatus(params[16]);
    
    om.update(update);
    core->setMechInfo(om);

    core->flushDisplay();
  }
  if (gsProtect) {
    gsCount-=LAG_PROTECT_TIME;
    if (gsCount<=0) {
      gsTime-=gsCount/3-1;
      core->slotLog(QString("LAG Protect: setting gsTime to %1").arg(gsTime));
      gsCount=1;
    }
    gsProtect=false;
  }
}


void HUDParser::parseStaticGeneralInfo(const QString & line) {
  QStringList params=QStringList::split(',',line,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if (params.count()>=9) {
    QString  oid = core->getOwnId();
    // ignore when own mechid is unknown
    MechInfo om = core->getMechInfo(core->getOwnId());
    if(om.isValid()) {
      MechInfo old_om = om;
      MechInfo update;

      update.setType(params[0][0]);
      update.setName(params[2]);
      double walk=params[4].toDouble();
      double run=params[3].toDouble();
      if (walk>run)
	std::swap(walk,run);
      update.setWalkingSpeed(walk);
      update.setRunningSpeed(run);
      update.setBackingSpeed(params[5].toDouble());

      om.update(update);
      core->setMechInfo(om);

      // why this???
      //      core->updateMaxSpeed(om.getRunningSpeed(),
      //			   om.getWalkingSpeed(),
      //			   om.getBackingSpeed());

      //      if(old_om.getType() != om.getType())
      //	core->cliffingsChanged();
      core->flushDisplay();
    }
  }
}


/**
   hudinfo c

   response:
   Zero or more:
  	#HUD:<key>:C:L# ID,AC,SE,UT,MN,X,Y,Z,RN,BR,SP,VS,HD,JH,RTC,BTC,TN,HT,FL
   Exactly once:
  	#HUD:<key>:C:D# Done

   ID: mechid, ID of the unit
   AC: arc, weapon arc the unit is in
   SE: sensors, sensors that see the unit
   UT: unit type character
   MN: string, mechname of unit, or '-' if unknown
   X, Y, Z: coordinates of unit
   RN: range, range to unit
   BR: degree, bearing to unit
   SP: speed, speed of unit
   VS: speed, vertical speed of unit
   HD: degree, heading of unit
   JH: degree, jump heading, or '-' if not jumping
   RTC: range, range from unit to X,Y center
   BTC: degree, bearing from unit to X,Y center
   TN: integer, unit weight in tons
   HT: heatmeasure, unit's apparent heat (overheat)
   FL: unit status string

*/

void HUDParser::parseContacts(const QString & con) {
  if (con.startsWith("E#")) { // ERROR
    core->slotLog(con.mid(3));
  } else if (con.startsWith("D#")) { // DONE
    core->updateMechsLOS(currentMechs);
    currentMechs.clear();

    if (conProtect) {
      conCount-=LAG_PROTECT_TIME;
      if (conCount<=0) {
	conTime-=conCount/3-1;
	core->slotLog(QString("LAG Protect: setting conTime to %1").arg(conTime));
	conCount=1;
      }
      conProtect=false;
    }
    
  } else if (con.startsWith("L#")) { // CONTACT LINE
    QStringList params=QStringList::split(',',con.mid(3),true);
    for (unsigned int pi=0; pi<params.count(); ++pi) 
      if (params[pi]=="-") 
	params[pi]="";

    MechInfo update;
    update.setId(params[0]);
    MechInfo::Arc arc = MechInfo::UNDEF_ARC;
    if(params[1].contains('*'))
      arc = (MechInfo::Arc)(arc | MechInfo::FORWARD_ARC);
    if(params[1].contains('r'))
      arc = (MechInfo::Arc)(arc | MechInfo::RIGHT_ARC);
    if(params[1].contains('l'))
      arc = (MechInfo::Arc)(arc | MechInfo::LEFT_ARC);
    if(params[1].contains('t'))
      arc = (MechInfo::Arc)(arc | MechInfo::TURRET_ARC);
    if(params[1].contains('v'))
      arc = (MechInfo::Arc)(arc | MechInfo::REAR_ARC);
    update.setArc(arc);

    MechInfo::SensorHit sensors = MechInfo::NO_SENSOR;
    if(params[2].contains('P'))
      sensors = (MechInfo::SensorHit)(sensors | MechInfo::PRIMARY_SENSOR);
    if(params[2].contains('S'))
      sensors = (MechInfo::SensorHit)(sensors | MechInfo::SECONDARY_SENSOR);
   // Force update of last in LOS Timestamp, esp. for Observation lounges etc
//   if(sensors != MechInfo::NO_SENSOR)
    update.setInLOS(true); 
    update.setSensorHits(sensors);

    update.setType(params[3][0]);
    update.setName(params[4]);
    double corr = 180.0 + params[15].toDouble();
    if(corr >= 360.0)
      corr -= 360.0;
    update.setPos(SubHEXPos(params[5].toInt(),params[6].toInt(),
			    params[7].toInt(),corr*M_PI/180.0,
			    params[14].toDouble()));
    update.setSpeed(params[10].toDouble());
    update.setVSpeed(params[11].toDouble());
    update.setHeading(params[12].toDouble()/180*M_PI);
    if(params[13] != "-") 
      update.setJumpHeading(params[13].toDouble()/180*M_PI);

    update.setWeight(params[16].toInt());
    update.setHeat(params[17].toInt());
    update.setStatus(params[18]);

    currentMechs[params[0].upper()]=update;
  }
}
void HUDParser::parseTacticalStart(const QString & coords) {
  QStringList params=QStringList::split(',',coords,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if (params.count()>=4) {
    tacLeftUpper=HEXPos(params[0].toInt(),params[1].toInt());
    tacRightLower=HEXPos(params[2].toInt(),params[3].toInt());
    //    core->slotLog("Tactical INFO Start: "+coords);
  }
}

void HUDParser::parseTacticalLine(const QString & line) {
  QStringList params=QStringList::split(',',line,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if (params.count()>=2) {
    int line=params[0].toInt();
    if (line>=tacLeftUpper.getY() && line <= tacRightLower.getY()) {
      QTextIStream str(&params[1]);
      HEXPos hpos(tacLeftUpper.getX(),line);
      while (!str.atEnd() && hpos.getX()<=tacRightLower.getX()) {
	MapTile mapTile;
	str >> mapTile;
	core->setMapTile(hpos,mapTile);
	//	core->slotLog(QString("Set HEX (%1,%2) -> (%3)").arg(hpos.getX())
	//			      .arg(hpos.getY()).arg(mapTile.getType()));
	hpos.setX(hpos.getX()+1);
      }
    }
  }
}

/*
c. Weapon List

   command:
	hudinfo wl

   repsonse:
   Zero or more:
	#HUD:<key>:WL:L# WN,NM,NR,SR,MR,LR,NS,SS,MS,LS,CR,WT,DM,RT,FM,AT,DT,HT
   Exactly one:
	#HUD:<key>:WL:D# Done

   WN: integer, weapon number
   NM: string, weapon name
   NR: range, minimum range
   SR: range, short range
   MR: range, medium range
   LR: range, long range
   NS: range, minimum range in water
   SS: range, short range in water
   MS: range, medium range in water
   LS: range, long range in water
   CR: integer, size in critslots
   WT: integer, weight in 1/100 tons
   DM: integer, maximum damage
   RT: integer, recycle time in ticks
   FM: weapon fire mode, possible fire modes
   AT: ammo type, possible ammo types
   DT: damage type
   HT: heat measure, weapon heat per salvo
*/
void HUDParser::parseWeaponListLine(const QString& line) {
  QStringList params=QStringList::split(',',line,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if(params.count() >= 13) {
    WeaponStat update;
    update.setId(params[0].toInt());
    if(params[1][2] == '.') {
      update.setName(params[1].mid(3));
      update.setTechName(params[1].left(2));
    } else {
      update.setName(params[1]);
    }
    update.setMinRange(params[2].toInt());
    update.setShortRange(params[3].toInt());
    update.setMidRange(params[4].toInt());
    update.setLongRange(params[5].toInt());
    update.setDamage(params[12].toInt());

    if (params.count() >= 17) { // hudinfo 0.7
      update.setHeat(params[17].toInt());
    }
    WeaponStat ws = core->getWeaponStat(params[0].toInt());
    ws.update(update);
    core->addWeaponStat(ws);
  }
}

/*
b. Weapon Status

   command:
	hudinfo we

   response:
   Zero or more:
	#HUD:<key>:WE:L# WN,WT,WQ,LC,ST,FM,AT
   Exactly once:
	#HUD:<key>:WE:D# Done

   WN: integer, weapon number
   WT: weapon type number
   WQ: weapon quality
   LC: section, location of weapon
   ST: weapon status
   FM: weapon fire mode
   AT: ammo type, the type of ammo selected
*/
void HUDParser::parseWeaponStatusLine(const QString& line) {
  QStringList params=QStringList::split(',',line,true);
  for (unsigned int pi=0; pi<params.count(); ++pi) 
    if (params[pi]=="-") 
      params[pi]="";
  if(params.count() >= 7) {
    WeaponStat ws = core->getWeaponStat(params[1].toInt());

    WeaponInfo update;
    update.setId(params[0].toInt());

    // we need this because the text parser doesn't
    // know the id, but only the name (techname + name)
    update.setName(ws.getName());
    update.setTechName(ws.getTechName());
    
    bool ok;
    int recycle = params[4].toInt(&ok);
    if(ok)
      update.setRecycleTime(recycle);
    else 
      update.setStatus(params[4]);

    update.setSection(params[3]);
    update.setAmmoType(params[6]);

    WeaponInfo wi = core->getWeaponInfo(params[0].toInt());
    wi.update(update);
    core->setWeaponInfo(params[0].toInt(), wi);

    if(maxWeaponId < params[0].toInt())
      maxWeaponId = params[0].toInt();
  }
}

/**

g. Original Armor Status

   command:
        hudinfo oas

   response:
   Zero or more:
        #HUD:<key>:OAS:L# SC,AF,AR,IS
   Exactly once:
        #HUD:<key>:OAS:D# Done

   SC: section
   AF: integer, original front armor or '-' if n/a
   AR: integer, original rear armor or '-' if n/a
   IS: integer, original internal structure or '-' if n/a

   Example:
    > hudinfo oas
    < #HUD:C58x2:OAS:L# LA,22,-,12
    < #HUD:C58x2:OAS:L# RA,22,-,12
    < #HUD:C58x2:OAS:L# LT,17,8,16
    < #HUD:C58x2:OAS:L# RT,17,8,16
    < #HUD:C58x2:OAS:L# CT,35,10,23
    < #HUD:C58x2:OAS:L# LL,18,-,16
    < #HUD:C58x2:OAS:L# RL,18,-,16
    < #HUD:C58x2:OAS:L# H,9,-,3
    < #HUD:C58x2:OAS:D# Done
*/

void HUDParser::parseOriginalArmorStatus(const QString & line) {
  if (line.startsWith("L#")) {
 
    QStringList params=QStringList::split(',',line.mid(3),true);
    for (unsigned int pi=0; pi<params.count(); ++pi) 
      if (params[pi]=="-") 
        params[pi]="0";
    if(params.count() >= 4) {
       workOriginalArmor[params[0]]=params[1].toInt();
       workOriginalArmor[params[0]+"-I"]=params[3].toInt();
       workOriginalArmor[params[0]+"-R"]=params[2].toInt();
    } 
  } else if (line.startsWith("D#")) {
    // ignore when own mechid is unknown
    MechInfo om = core->getMechInfo(core->getOwnId());
    if(om.isValid()) {
      MechInfo update;
      update.setOriginalArmor(workOriginalArmor);
      om.update(update);
      core->setMechInfo(om);
      workOriginalArmor.clear();
      // We only change armor status, no need to flush them.
      // core->flushDisplay();
    }
  }
}
/**
h. Armor Status

   command:
        hudinfo as
   response:
   Zero or more:
        #HUD:<key>:AS:L# SC,AF,AR,IS
   Exactly once:
        #HUD:<key>:AS:D# Done

   SC: section
   AF: integer, front armor or '-' if n/a
   AR: integer, rear armor or '-' if n/a
   IS: integer, internal structure or '-' if n/a, 0 if destroyed

   Example:
    > hudinfo as
    < #HUD:C58x2:AS:L# LA,0,-,0
    < #HUD:C58x2:AS:L# RA,22,-,12
    < #HUD:C58x2:AS:L# LT,9,5,16
    < #HUD:C58x2:AS:L# RT,4,1,16
    < #HUD:C58x2:AS:L# CT,0,10,5
    < #HUD:C58x2:AS:L# LL,0,-,16
    < #HUD:C58x2:AS:L# RL,18,-,16
    < #HUD:C58x2:AS:L# H,9,-,3
    < #HUD:C58x2:AS:D# Done



*/
void HUDParser::parseArmorStatus(const QString & line) {
  if (line.startsWith("L#")) {
 
    QStringList params=QStringList::split(',',line.mid(3),true);
    for (unsigned int pi=0; pi<params.count(); ++pi) 
      if (params[pi]=="-") 
        params[pi]="0";
    if(params.count() >= 4) {
       workCurrentArmor[params[0]]=params[1].toInt();
       workCurrentArmor[params[0]+"-I"]=params[3].toInt();
       workCurrentArmor[params[0]+"-R"]=params[2].toInt();
    } 
  } else if (line.startsWith("D#")) {
    // ignore when own mechid is unknown
    MechInfo om = core->getMechInfo(core->getOwnId());
    if(om.isValid()) {
      MechInfo update;
      update.setCurrentArmor(workCurrentArmor);
      om.update(update);
      core->setMechInfo(om);
      workCurrentArmor.clear();
      // We only change armor status, no need to flush them.
      // core->flushDisplay();
    }
  }

}

void HUDParser::flush() {
  // first some special initializing cases
  if (serverVersion.isEmpty()) {
    if (serverVersionRequested==0) {
      core->slotSend("hudinfo\n");
      core->slotLog("Requesting hudinfo version");

      serverVersionRequested=10*4; // request approx. every 10 sec
    } else {
      --serverVersionRequested;
    }
  } else if (sessionKey.isEmpty()) {
    // TODO: generate session key
    sessionKey="xperthud";
    
    core->slotSend("hudinfo key="+sessionKey+"\n");
    core->slotLog("Setting Session key to: "+sessionKey);
    
    active = true;

  } else { // Normal sending stuff
    --gsCount;
    if(gsCount == 0 && gsTime>0) {
      gsCount = gsTime+LAG_PROTECT_TIME;
      core->slotSend("hudinfo gs\n");
      gsProtect=true;
      ++sgiCount;
      if (sgiCount % 5 == 1) {
         core->slotSend("hudinfo as\n");
      }
      if (sgiCount == 4) {
         core->slotSend("hudinfo oas\n");
      }
      if(sgiCount == 2) {
	core->slotSend("hudinfo sgi\n");
      }
      if (sgiCount >=20) {
        sgiCount=0;
      }
    }
    --tacCount;
    if(tacCount == 0 && tacTime>0) {
      tacCount = tacTime+LAG_PROTECT_TIME;
      core->slotSend("hudinfo t 20\n");
      tacProtect=true;
    }
    --conCount;
    if (conCount == 0 && conTime>0) {
      conCount = conTime+LAG_PROTECT_TIME;
      core->slotSend("hudinfo c\n");
      conProtect=true;
    }

    --weCount;
    if (weCount == 0 && weTime>0) {
      if(!hasWeaponList) {
	weCount = weTime;
	core->slotSend("hudinfo wl\n");
      } else {
	weCount = weTime+LAG_PROTECT_TIME;
	core->slotSend("hudinfo we\n");
	weProtect=true;
      }
    }
  }
}


