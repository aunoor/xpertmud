#include "TextParser.h"
#include <iostream>
using std::endl;
using std::cout;

const int TextParser::LAG_PROTECT_TIME = 120;


TextParser::TextParser(BattleCore * core):
  BattleParser(core), 
  gsTime(0), gsCount(0), gsProtect(false),
  tacTime(0), tacCount(0), tacProtect(false),
  conTime(0), conCount(0), conProtect(false),
  weTime(0), weCount(0), weProtect(false),
  initializeRequested(8), initialized(false),
  doTactical(true), doContacts(true), doStatus(true),
  inContacts(false),
  inTactical(false), tactCurrentX(0), tactCurrentY(0),
  inWeapons(false),
  maxWeaponId(-1)
{
  active = false;
  tactNumbers = 
    QRegExp("^     (\\d)  (\\d)  (\\d)");
  tactStart =
    QRegExp("(^    (\\\\([^\\/]+)\\/([^\\\\]+))+\\\\([^\\/]+)\\/$)|(^    (\\/([^\\\\]+)\\\\([^\\/]+))+\\/([^\\\\]+)\\\\$)|(^    (\\\\([^\\/]+)\\/([^\\\\]+))+\\\\$)|(^    (\\/([^\\\\]+)\\\\([^\\/]+))+\\/$)");
  tactFirstNumber = QRegExp("\\s*(\\d+) ");
  tactFirstEmpty = QRegExp("    ");
  tactEndLine = QRegExp("^[\\/][^\\/]*$");
  tactUpperHex = QRegExp("^\\/..");
  tactLowerHex = QRegExp("^\\\\..");

  contBegin = QRegExp("^Line of Sight Contacts:");
  contLine = 
    QRegExp("^...\\[..\\]. ............ x:... y:... z:... r:..\\.. b:... s:...\\.. h:... S:..");
  contEnd = QRegExp("^End Contact List");

  statXYZExcessHeat = QRegExp("^X, Y, Z:.*Excess Heat:.*Heat Production");
  statHeatSt = QRegExp("^X, Y, Z:.*Excess Heat:.*Heat Production:\\s+\\d+");
  statBeforeHeatSt = QRegExp("^X, Y, Z:.*Excess Heat:.*Heat Production:\\s+");

  statSpeedHeatDissipation = QRegExp("^Speed:\\s+-?\\d+\\s+KPH\\s+Heading:\\s+");
  statSpeedCurrent = QRegExp("^Speed:\\s+-?\\d+");
  statHeadingSt = QRegExp("^Speed:.*KPH\\s+Heading:\\s+\\d+");
  statBeforeHeadingSt = QRegExp("^Speed:.*KPH\\s+Heading:\\s+");
  statHeatDissipation = QRegExp("^Speed:.*KPH.*\\sdeg\\s+Heat Sinks:\\s+\\d+");
  statBeforeHeatDissipation = QRegExp("^Speed:.*KPH.*\\sdeg\\s+Heat Sinks:\\s+");
  statSpeedInfo = QRegExp("^Tonnage:\\s+\\d+\\s+MaxSpeed:\\s+-?\\d+");
  statBeforeSpeedInfo = QRegExp("^Tonnage:\\s+\\d+\\s+MaxSpeed:\\s+");
  statDesSpeedInfo = QRegExp("^Des. Speed:\\s+-?\\d+");
  statMechInfo = QRegExp("[^\\s]+\\sName: .*ID:\\[..\\]\\s+");
  statBeforeName = QRegExp("[^\\s]+\\sName: ");
  statBeforeId = QRegExp(".*ID:\\[");
  statFindcenter = QRegExp("^Current hex: \\(\\d+,\\d+,-?\\d+");
  statSpeed = QRegExp("^Your current speed is");
  statHeading = QRegExp("^Your current heading is");
  statHeat = QRegExp("Temp:.");

  weapStart = QRegExp("^------ Weapon -+\\s+\\[##\\]");
  weapEntry = QRegExp("^ [^\\[]+\\[..\\]\\s+[^\\s]([^\\s]|\\s[^\\s])+\\s+[^\\s]+\\s+\\|\\|(\\s+[^\\s]([^\\s]|\\s[^\\s])+\\s+[^\\s\\d]([^\\s\\d]|\\s[^\\s\\d])*\\s+\\d+\\s*)?");
  weapName = QRegExp("^ [^\\[]+");
  weapBeforeNr = QRegExp("^\\[");
  weapBeforeLocation = QRegExp("^..\\]\\s+");
  weapLocation = QRegExp("^[^\\s]([^\\s]|\\s[^\\s])+");
  weapBeforeStatus = QRegExp("^\\s+");
  weapStatus = QRegExp("^[^\\s]+");
  weapAmmoBeforeName = QRegExp("^\\s+\\|\\|\\s+");
  weapAmmoName = QRegExp("^[^\\s]([^\\s]|\\s[^\\s])+");
  weapAmmoBeforeStatus = QRegExp("^\\s+");
  weapAmmoStatus = QRegExp("^[^\\s\\d]([^\\s\\d]|\\s[^\\s\\d])*");
  weapAmmoBeforeAmount = QRegExp("^\\s+");
  weapAmmoAmount = QRegExp("^\\d+");

  wsLine = 
    QRegExp("^[\\w.]+\\s+\\d+\\s+\\d+\\s+\\d+\\s+\\d+\\s+\\d+\\s+\\d+\\s+\\d+\\s*$");
  wsName = QRegExp("^[\\w.]+");
  wsSpace = QRegExp("^\\s+");
  wsNumber = QRegExp("^\\d+");

  initExp = QRegExp("^&GOTFIRST .*=1");
  
  spaces = QRegExp("\\s+$");
}

void TextParser::lagProtect(int& count, int& time, bool& protect,
			    const QString& description) {
  if(protect) {
    count -= LAG_PROTECT_TIME;
    if(count <= 0) {
      time -= count/3-1;
      core->slotLog(QString("TextParser LAG Protect: setting %1 to %2")
		    .arg(description).arg(time));
      count = 1;
    }
    protect = false;
  }
}

bool TextParser::stepTimer(int& count, int time, bool& protect) {
  --count;
  if(count == 0 && time > 0) {
    count = time + LAG_PROTECT_TIME;
    protect = true;
    return true;
  }
  return false;
}

void TextParser::parse(const QString& text) {
#if QT_VERSION<300
  if(initExp.find(text,0) != -1) {
#else
  if(initExp.search(text) != -1) {
#endif
    active = true;
    core->slotSend("@set/quiet me=GOTFIRST:0\n");
  }

  if(doTactical) {
    parseTactical(text);
  }
  if(doContacts) {
    parseContacts(text);
  }
  if(doStatus) {
    parseStatus(text);
  }
}

void TextParser::flush() {
  if(!active) {
    if(initializeRequested == 0) {
      core->slotLog("Initializing TextParser commands.");

      core->slotSend("@set/quiet me=GOTFIRST:1\n");
      //      core->slotSend("@set/quiet me=Tacheight:15\n");
      //      core->slotSend("&HUD_TACTICAL me=$hud_tactical * *:@set/quiet me=HD_INT:{tactical %0 %1 ;|@pemit me=SSKIP%%r%%|%%rESKIP};@trigger/quiet me/HD_INT\n");

      core->slotSend("@set/quiet me=HUD_TACTICAL:$hud_tactical * *:@fo me={tactical %0 %1;|@pemit me=SSKIP\\%r\\%|\\%rESKIP}\n");
      core->slotSend("@set/quiet me=HUD_FINDCENTER:$hud_findcenter:findcenter;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_CONTACTS:$hud_contacts:contacts;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_STATUS:$hud_status:status;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_STATUSI:$hud_statusi:status i;|@pemit me=SSKIP%r%|ESKIP\n");
      core->slotSend("@set/quiet me=HUD_STATUSW:$hud_statusw:status w;|@pemit me=SSKIP%r%|ESKIP\n");
      core->slotSend("@set/quiet me=HUD_HEAT:$hud_heat:status h;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_SPEED:$hud_speed:speed;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_HEADING:$hud_heading:heading;|@pemit me=SSKIP%r%|%rESKIP\n");
      core->slotSend("@set/quiet me=HUD_DECOMPILE:$hud_decompile:@decompile me/GOTFIRST;|@pemit me=SSKIP%r%|%rESKIP\n");
//      core->slotSend("hud_decompile\n");

      initializeRequested=10*4; // request approx. every 10 sec
    } else {
      --initializeRequested;
    }
  } else {
    if(stepTimer(gsCount, gsTime, gsProtect)) {
      core->slotSend("hud_statusi\n");
      core->slotSend("hud_findcenter\n");
    }
    if(stepTimer(conCount, conTime, conProtect)) {
      core->slotSend("hud_contacts\n");
    }
    if(stepTimer(weCount, weTime, weProtect)) {
      core->slotSend("hud_statusw\n");
    }
  }
}

void TextParser::parseTactical(const QString& text) {
  //  cout << "TextParsing: " << text << endl;
  // lets look if this tells us our left upper x
  if(!inTactical) {
#if QT_VERSION<300
    if(tactNumbers.find(text,0) != -1) {
#else
    if(tactNumbers.search(text) != -1) {
#endif
      tactCurrentX = (tactCurrentX * 10) + (text[5]-'0'); 
      //      cout << "Found the current X " << tactCurrentX << endl;
#if QT_VERSION<300
    } else if(tactStart.find(text,0) != -1) {
#else
    } else if(tactStart.search(text) != -1) {
#endif
      inTactical = true;
      //      cout << "Found begin of tactical" << endl;
    } else {
      tactCurrentX = 0;
    }
  } else {
    QString work(text);
#if QT_VERSION<300
    if(tactFirstNumber.find(work,0) != -1) {
#else
    if(tactFirstNumber.search(work) != -1) {
#endif
      tactCurrentY = work.left(3).toInt();
      //      cout << "Found a line with a number: " << tactCurrentY << endl;
      work.replace(tactFirstNumber, "");
    } else {
      work.replace(tactFirstEmpty, "");
    }
    int x = tactCurrentX;
    while(work != "") {
      int replaced = 0;
      //      cout << work << endl;
#if QT_VERSION<300
      if(tactEndLine.find(work,0) != -1) {
#else
      if(tactEndLine.search(work) != -1) {
#endif
	//	cout << "Found the line's end" << endl;
	break;
      }
      //      cout << work << endl;
#if QT_VERSION<300
      if(tactUpperHex.find(work,1) != -1) {
#else
      if(tactUpperHex.search(work) != -1) {
#endif
	char left = work.latin1()[1];
	if(left == ']')
	  inTactical = false;
	work.replace(tactUpperHex, "");
	++x;
	++replaced;
      }
      //      cout << work << endl;
#if QT_VERSION<300
      if(tactEndLine.find(work,0) != -1) {
#else
      if(tactEndLine.search(work) != -1) {
#endif
	//	cout << "Found the line's end" << endl;
	break;
      }
      //      cout << work << endl;
#if QT_VERSION<300
      if(tactLowerHex.find(work,1) != -1) {
#else
      if(tactLowerHex.search(work) != -1) {
#endif
	char left = work.latin1()[1];
        char right = work.latin1()[2];
	int height = 0;
	if(left != right) {
	  height = right-'0';

	  // TODO: Check weather we need this:
	  //	  if(left == '~')
	  //height = -height;
	}
	if(left == '_')
	  left = '.';
	//	cout << "Found HEX (" << x << ", "
	//	     << tactCurrentY << "): " 
	//	     << left << " " << height << endl;
	core->setMapTile(HEXPos(x, tactCurrentY),
			 MapTile(left, height));
	work.replace(tactLowerHex, "");
	++x;
	++replaced;
      }
      //      cout << work << endl;
      if(replaced == 0) {
	cout << "Couldn't parse line!" << endl;
	inTactical = false;
	work = "";
      }
    }
    if(!inTactical)
      core->flushDisplay();
  }
}

void TextParser::parseContacts(const QString& text) {
  if(!inContacts) {
#if QT_VERSION<300
    if(contBegin.find(text,0) != -1) {
#else
    if(contBegin.search(text) != -1) {
#endif
      inContacts = true;
      //      cout << "Found start of contact list" << endl;
    }
  } else {
#if QT_VERSION<300
    if(contLine.find(text,0) != -1) {
#else
    if(contLine.search(text) != -1) {
#endif
      MechInfo update;

      MechInfo::SensorHit sensorHit;
      if(text.latin1()[0] == 'P')
	sensorHit = MechInfo::PRIMARY_SENSOR;
      else if(text.latin1()[0] == 'S')
	sensorHit = MechInfo::SECONDARY_SENSOR;
      else
	sensorHit = MechInfo::NO_SENSOR;
      // if we get a mech in contacts it's always in los...
      //      if(sensorHit != MechInfo::NO_SENSOR) {
      update.setInLOS(true);
      //      }
      update.setSensorHits(sensorHit);

      MechInfo::Arc arc = MechInfo::UNDEF_ARC;
      QString arcs = text.mid(1, 2);
      if(arcs.contains('*'))
	arc = (MechInfo::Arc)(arc | MechInfo::FORWARD_ARC);
      if(arcs.contains('r'))
	arc = (MechInfo::Arc)(arc | MechInfo::RIGHT_ARC);
      if(arcs.contains('l'))
	arc = (MechInfo::Arc)(arc | MechInfo::LEFT_ARC);
      if(arcs.contains('t'))
	arc = (MechInfo::Arc)(arc | MechInfo::TURRET_ARC);
      if(arcs.contains('v'))
	arc = (MechInfo::Arc)(arc | MechInfo::REAR_ARC);
      update.setArc(arc);

      update.setId(text.mid(4, 2));
      update.setType(text.latin1()[7]);

      QString name = text.mid(9, 12);
      name.replace(spaces, "");
      update.setName(name);

      // not really needed... but let this in, perhaps
      // we'll need it somewhen
      //      int x = text.mid(24, 3).toInt();
      //      int y = text.mid(30, 3).toInt();
      //      int z = text.mid(36, 3).toInt();

      update.setSpeed(text.mid(55, 5).toDouble());
      update.setHeading((double)text.mid(63, 3).toInt()*M_PI/180.0);
      QString status;
      if(text.length() >= 70)
	update.setStatus(text.mid(69, 2));
      else if(text.length() >= 69)
	update.setStatus(text.mid(69, 1));

      double r = text.mid(42, 4).toDouble();
      int b = text.mid(49, 3).toInt();
      SubHEXPos pos = core->getMechInfo(core->getOwnId()).getPos();

      //      cout << "ID: " << core->getOwnId() << "; POS: " 
      //	   << pos.getX() << ", " << pos.getY() << endl;

      //cout << "Calc pos from b: " << b << "; r: " << r << endl;
      SubPos difference((double)b*M_PI/180.0, r);
      pos += difference;
      update.setPos(pos);
      
      currentMechs[update.getId().upper()] = update;  
 
      //      cout << currentMechs[update.getId().upper()].toString
      //	(currentMechs[update.getId().upper()]) << endl;

#if QT_VERSION<300
    } else if(contEnd.find(text,0) != -1) {
#else
    } else if(contEnd.search(text) != -1) {
#endif
      core->updateMechsLOS(currentMechs);
      currentMechs.clear();
      inContacts = false;
      //cout << "Found end of contact list" << endl;

      lagProtect(conCount, conTime, conProtect, "contacts");
      core->flushDisplay();
    }
  }
}

void TextParser::parseStatus(const QString& text) {
  //  cout << "parsing "  << text << endl;
  if(!inWeapons) {
    //cout << "not in weapons " << endl;
#if QT_VERSION<300
    if(statMechInfo.find(text,0) != -1) {
#else
    if(statMechInfo.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statBeforeName, "");
      
      QString work2 = work;
      work2.replace(statBeforeId, "");
      
      QString name = work.left(work.length()-work2.length()-4);
      QString ref = work2.left(2);

      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;
      update.setId(ref.lower());
      update.setName(name);
      update.setType('B');
      om.update(update);
      core->setOwnId(ref.lower());
      core->setMechInfo(om);

#if QT_VERSION<300
    } else if(statFindcenter.find(text,0) != -1) {
#else
    } else if(statFindcenter.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statFindcenter, "");

      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;

      QStringList coords = 
	QStringList::split(',', text.mid(14, text.length()-work.length()-14));
      int x = coords[0].toInt();
      int y = coords[1].toInt();
      int z = coords[2].toInt();
      //      cout << "\"" << work.mid(17, 4) << "\"" << endl;
      double r = work.mid(18, 6).toDouble();
      int b = work.mid(42, 4).toInt();

      update.setPos(SubHEXPos(x, y, z, (double)b/180.0*M_PI-M_PI, r));
      //      cout << "POS: " << update.getPos().getX() << ", "
      //	   << update.getPos().getY() << endl;
      update.setInLOS(true);
      om.update(update);
      core->setMechInfo(om);

      //      cout << "Findcenter (" << x << ", " << y << ", " << z << ") " 
      //	   << "Range: " << r << ", Bearing: " << b
      //	   << endl;
      lagProtect(gsCount, gsTime, gsProtect, "findcenter");
      core->flushDisplay();

#if QT_VERSION<300
    } else if(statHeading.find(text,0) != -1) {
#else
    } else if(statHeading.search(text) != -1) {
#endif
      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;

      int h = text.mid(24, text.length()-24-1).toInt(); // see the point...
      //      cout << "Heading: " << h << endl;

      update.setHeading((double)h/180.0*M_PI);
      om.update(update);
      core->setMechInfo(om);

#if QT_VERSION<300
    } else if(statSpeed.find(text,0) != -1) {
#else
    } else if(statSpeed.search(text) != -1) {
#endif
      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;

      double s = text.mid(22, text.length()-22-1).toDouble(); // point.

      update.setSpeed(s);
      om.update(update);
      core->setMechInfo(om);
      /*
#if QT_VERSION<300
    } else if(statHeat.find(text,0) != -1) {
#else
    } else if(statHeat.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statHeat, "");
      const char *cwork = work.latin1();

      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;

      int heat = 0;
      for(unsigned int i=0; i<work.length(); ++i) {
	if(cwork[i] == ':') {
	  ++heat;
	}
      }
      
      update.setHeat(heat*10);
      om.update(update);
      core->setMechInfo(om);
      */
#if QT_VERSION<300
    } else if(statSpeedInfo.find(text,0) != -1) {
#else
    } else if(statSpeedInfo.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statSpeedInfo, "");
      QString work2 = text;
      work2.replace(statBeforeSpeedInfo, "");
      QString maxSpeedS = text.mid(text.length()-work2.length(),
				   work2.length()-work.length());
      //      cout << "MaxSpeed: " << maxSpeedS << endl;
      double maxSpeed = maxSpeedS.toDouble();
      
      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;
      update.setRunningSpeed(maxSpeed);
      update.setWalkingSpeed(maxSpeed*2.0/3.0);
      update.setBackingSpeed(-maxSpeed*2.0/3.0);
      om.update(update);
      core->setMechInfo(om);

#if QT_VERSION<300
    } else if(statDesSpeedInfo.find(text,0) != -1) {
#else
    } else if(statDesSpeedInfo.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statDesSpeedInfo, "");
      QString desSpeedS = text.mid(11, text.length()-work.length()-11);
      //      cout << "DesSpeed: " << desSpeedS << endl;
      double desSpeed = desSpeedS.toDouble();
      
      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;
      update.setDesiredSpeed(desSpeed);
      om.update(update);
      core->setMechInfo(om);
      
#if QT_VERSION<300
    } else if(statSpeedHeatDissipation.find(text,0) != -1) {
#else
    } else if(statSpeedHeatDissipation.search(text) != -1) {
#endif
      QString work = text;
      work.replace(statSpeedCurrent, "");
      QString speedS = text.mid(6, text.length()-work.length()-6);
      //      cout << "NewSpeed: " << speedS << endl;
      int speed = speedS.toInt();
      
      QString work2 = text;
      work2.replace(statHeadingSt, "");
      QString work3 = text;
      work3.replace(statBeforeHeadingSt, "");
      QString headingS = text.mid(text.length()-work3.length(),
				  work3.length()-work2.length());
      int heading = headingS.toInt();

      QString work4 = text;
      work4.replace(statHeatDissipation, "");
      QString work5 = text;
      work5.replace(statBeforeHeatDissipation, "");
      QString heatSinksS = text.mid(text.length()-work5.length(),
				    work5.length()-work4.length());
      //      cout << "HeatDissipation: " << heatSinksS << endl;
      int heatSinks = heatSinksS.toInt();

      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;
      update.setSpeed(speed);
      update.setHeading((double)heading*M_PI/180.0);
      update.setHeatDissipation(heatSinks);
      om.update(update);
      core->setMechInfo(om);

#if QT_VERSION<300
    } else if(statXYZExcessHeat.find(text,0) != -1) {
#else
    } else if(statXYZExcessHeat.search(text) != -1) {
#endif
      QString work2 = text;
      work2.replace(statHeatSt, "");
      QString work3 = text;
      work3.replace(statBeforeHeatSt, "");
      QString heatS = text.mid(text.length()-work3.length(),
			       work3.length()-work2.length());
      int heat = heatS.toInt();
      
      MechInfo om = core->getMechInfo(core->getOwnId());
      MechInfo update;
      update.setHeat(heat);
      om.update(update);
      core->setMechInfo(om);
      
#if QT_VERSION<300
    } else if(wsLine.find(text,0) != -1) {
#else
    } else if(wsLine.search(text) != -1) {
#endif
      QString work = text;
      work.replace(wsName, "");
      QString wiName = text.left(text.length()-work.length());
      work.replace(wsSpace, "");
      QString work2, parsed;
#define WINUMBER(X) \
      work2= work; \
      work.replace(wsNumber, ""); \
      parsed = work2.left(work2.length()-work.length()); \
      int X = parsed.toInt(); \
      work.replace(wsSpace, "");

      WINUMBER(wiHeat);
      WINUMBER(wiDamage);
      WINUMBER(wiMin);
      WINUMBER(wiShort);
      WINUMBER(wiMed);
      WINUMBER(wiLong);

      cout << "Name: " << wiName << "; Heat: " << wiHeat
	   << "; Damgage: " << wiDamage << "; Min: "
	   << wiMin << "; Short: " << wiShort << "; Med: "
	   << wiMed << "; Long: " << wiLong
	   << endl;
      WeaponStat update;
      if(wiName[2] == '.') {
	update.setTechName(wiName.left(2));
	update.setName(wiName.mid(3));

	// Now look weather the actual weapons have
	// the correct tech name:
	for(int i=0; i<core->getNrWeapons(); ++i) {
	  WeaponInfo wi = core->getWeaponInfo(i);
	  if(wi.hasName() && wi.getName() == wiName.mid(3)) {
	    wi.setTechName(wiName.left(2));
	    cout << "Setting techName " << wi.getTechName()
		 << " on weapon " << wi.getName() << endl;
	    core->setWeaponInfo(i, wi);
	  }
	}

      } else {
	update.setName(wiName);
      }
      update.setHeat(wiHeat*10); // convert to real heat value
      update.setDamage(wiDamage);
      update.setMinRange(wiMin);
      update.setShortRange(wiShort);
      update.setMidRange(wiMed);
      update.setLongRange(wiLong);

      WeaponStat ws = core->getWeaponStat(wiName);
      ws.update(update);
      core->addWeaponStat(ws);

#undef WINUMBER

#if QT_VERSION<300
    } else if(weapStart.find(text,0) != -1) {
#else
    } else if(weapStart.search(text) != -1) {
#endif
      //      cout << "Start of Weapon List" << endl;
      inWeapons = true;
    }
  } else {
#if QT_VERSION<300
    if(weapEntry.find(text,0) != -1) {
#else
    if(weapEntry.search(text) != -1) {
#endif
      QString work = text;
      QString without;
      work.replace(weapName, "");
      QString name = text.mid(1, text.length()-work.length()-1);
      name.replace(spaces, "");
      //      cout << "Found weapon: \"" << name << "\"" << endl;

      work.replace(weapBeforeNr, "");
      QString nrS = work.left(2);
      //      cout << "Nr: \"" << nrS << "\"" << endl;
      int nr = nrS.toInt();

      work.replace(weapBeforeLocation, "");
      without = work;
      without.replace(weapLocation, "");
      QString location = work.left(work.length()-without.length());
      //      cout << "Location: \"" << location << "\"" << endl;

      work.replace(weapLocation, "");
      work.replace(weapBeforeStatus, "");
      without = work;
      without.replace(weapStatus, "");
      QString status = work.left(work.length()-without.length());
      //      cout << "Status: \"" << status << "\"" << endl;

      WeaponInfo update;
      update.setId(nr);
      update.setName(name);
      
      bool ok;
      int recycle = status.toInt(&ok);
      if(ok)
	update.setRecycleTime(recycle);
      else
	update.setStatus(status.left(1));
      if(location == "Aft")
	update.setSection("A");
      else if(location == "Aft Side")
	update.setSection("AS");
      else if(location == "Cockpit")
	update.setSection("C");
      else if(location == "Center Torso")
	update.setSection("CT");
      else if(location == "Engine")
	update.setSection("E");
      else if(location == "Fuselage")
	update.setSection("F");
      else if(location == "Front Left Leg")
	update.setSection("FLL");
      else if(location == "Front Left Side")
	update.setSection("FLS");
      else if(location == "Front Right Leg")
	update.setSection("FRL");
      else if(location == "Front Right Side")
	update.setSection("FRS");
      else if(location == "Front Side")
	update.setSection("FS");
      else if(location == "Head")
	update.setSection("H");
      else if(location == "Left Arm")
	update.setSection("LA");
      else if(location == "Left Leg")
	update.setSection("LL");
      else if(location == "Left Rear Wing")
	update.setSection("LRW");
      else if(location == "Left Side")
	update.setSection("LS");
      else if(location == "Left Torso")
	update.setSection("LT");
      else if(location == "Left Wing")
	update.setSection("LW");
      else if(location == "Nose")
	update.setSection("N");
      else if(location == "Rotor")
	update.setSection("R");
      else if(location == "Right Arm")
	update.setSection("RA");
      else if(location == "Right Leg")
	update.setSection("RL");
      else if(location == "Rear Left Side")
	update.setSection("RLS");
      else if(location == "Rear Right Side")
	update.setSection("RRS");
      else if(location == "Right Rear Wing")
	update.setSection("RRW");
      else if(location == "Right Side")
	update.setSection("RS");
      else if(location == "Right Torso")
	update.setSection("RT");
      else if(location == "Right Wing")
	update.setSection("RW");
      else if(location == "Turret")
	update.setSection("T");
      else
	update.setSection(location);
      
      WeaponInfo wi = core->getWeaponInfo(nr);
      wi.update(update);
      core->setWeaponInfo(nr, wi);

      if(maxWeaponId < nr)
	maxWeaponId = nr;

      work.replace(weapStatus, "");
      work.replace(weapAmmoBeforeName, "");
      //      cout << "work: " << work << endl;
#if QT_VERSION<300
      if(weapAmmoName.find(work,0) != -1) {
#else
      if(weapAmmoName.search(work) != -1) {
#endif
	//	cout << "Ammo found..." << endl;
	without = work;
	without.replace(weapAmmoName, "");
	QString ammoName = work.left(work.length()-without.length());
	//	cout << "AmmoName: \"" << ammoName << "\"" << endl;
	
	work.replace(weapAmmoName, "");
	work.replace(weapAmmoBeforeStatus, "");
	without = work;
	without.replace(weapAmmoStatus, "");
	QString ammoStatus = work.left(work.length()-without.length());
	//	cout << "AmmoStatus: \"" << ammoStatus << "\"" << endl;
	
	work.replace(weapAmmoStatus, "");
	work.replace(weapAmmoBeforeAmount, "");
	without = work;
	without.replace(weapAmmoAmount, "");
	QString ammoAmountS = work.left(work.length()-without.length());
	//	cout << "AmmoAmount: \"" << ammoAmountS << "\"" << endl;

      } 
    } else {
      core->setNrWeapons(maxWeaponId+1);
      maxWeaponId = -1;
      lagProtect(weCount, weTime, weProtect, "status w");
      
      //      cout << "End of Weapon List" << endl;
      inWeapons = false;
      parseStatus(text);
    }
  }
}

