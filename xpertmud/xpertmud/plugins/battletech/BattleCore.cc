#include "BattleCore.h"
#include "HUDParser.h"
//#include "TextParser.h"

#include <cassert>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <map>
#include <kglobal.h>
#include <kstandarddirs.h>

const char MapTile::UNKNOWN_HEIGHT = 1;


QTextStream & operator<<(QTextStream & ts, const MapTile t) {
  ts << t.getType();
  if (t.hasHeight()) {
    ts << t.getHeight();
  } else {
    ts << '?';
  }
  return ts;
}

QTextStream & operator>>(QTextStream & ts, MapTile & t) {
  char type;
  char heightChar;
  ts >> type;
  // TODO: Check for validity (or implement check in setType)
  t.setType(type);
  ts >> heightChar;
  int height=heightChar - '0';
  t.setHeight(height);
  return ts;
}

MapTile::MapTile(QTextStream & ts) {
  // Could be done faster ...
  MapTile t;
  ts>>t;
  *this=t;
}


 bool BattleMap::setMapTile(const HEXPos & pos, const MapTile& mapTile) {
    if(pos.getY() >= (int)myMap.size()) {
      myMap.resize(pos.getY()+1);
      if (pos.getY() >= (int)height) 
	height = pos.getY()+1;
    }
    if(pos.getX() >= (int)myMap[pos.getY()].size()) {
      myMap[pos.getY()].resize(pos.getX()+1);
      if (pos.getX() >= (int)width)
	width = pos.getX()+1;
    }
    if (myMap[pos.getY()][pos.getX()] != mapTile) {
      if (mapTile.hasType())
	myMap[pos.getY()][pos.getX()].setType(mapTile.getType());
      if (mapTile.hasHeight())
	myMap[pos.getY()][pos.getX()].setHeight(mapTile.getHeight());
 
      HEXPos n_pos;

///////////
       n_pos=pos.N();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagN();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagS();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagN();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagS();  
	} 
      }
///////////
       n_pos=pos.NE();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagNE();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagSW();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagNE();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagSW();  
	} 
      }
///////////
       n_pos=pos.SE();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagSE();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagNW();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagSE();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagNW();  
	} 
      }
///////////
       n_pos=pos.S();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagS();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagN();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagS();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagN();  
	} 
      }
///////////
       n_pos=pos.SW();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagSW();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagNE();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagSW();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagNE();  
	} 
      }
///////////
       n_pos=pos.NW();
       if(n_pos.getY()>=0 && n_pos.getY() < (int)myMap.size()  &&
          n_pos.getX()>=0 && n_pos.getX() < (int)myMap[n_pos.getY()].size()) {


          if (myMap[n_pos.getY()][n_pos.getX()].getKind() ==
		 myMap[pos.getY()][pos.getX()].getKind()) {

            myMap[pos.getY()][pos.getX()].setSameFlagNW();
            myMap[n_pos.getY()][n_pos.getX()].setSameFlagSE();  
        } else {
            myMap[pos.getY()][pos.getX()].resetSameFlagNW();
            myMap[n_pos.getY()][n_pos.getX()].resetSameFlagSE();  
	} 
      }

      return true;
    }
    return false;
  }


BattleMap::BattleMap(const QString& id, 
		     unsigned int width, 
		     unsigned int height):
  id(id),width(width),height(height) {
  resize(width, height);
}

BattleMap::BattleMap(const QString& id, 
		     QTextStream& stream):
  id(id),width(0),height(0) {
  load(stream);
}

void BattleMap::resize(unsigned int width, unsigned int height)
{
  if(height > 0) {
    if(width > 0) {
      myMap.resize(size_t(height), QValueVector<MapTile>(size_t(width)));
    } else {
      myMap.resize(size_t(height));
    }
  } else {
    myMap.clear();
  }
}

void BattleMap::load(QTextStream& stream) {
  stream >> width >> height;
  resize(width, height);
  for(unsigned int y=0; y<height; ++y) {
    for(unsigned int x=0; x<width; ++x) {
      MapTile t;
      stream >> t;
      setMapTile(HEXPos(x, y), t);
    }
  }
}

void BattleMap::save(QTextStream& stream) {
  stream << width << " " << height << "\n";
  for(unsigned int y=0; y<height; ++y) {
    for(unsigned int x=0; x<width; ++x) {
      stream << getMapTile(HEXPos(x, y));
    }
    stream << "\n";
  }
}


BattleCore::BattleCore()
  :refcount(0),
   hudParser(new HUDParser(this)),
   textParser(NULL /* new TextParser(this) */),
   current(new BattleMap("UNKNOWN1")),
   ownId("")
{
  KGlobal::dirs()->addResourceType
    ("btech-maps", "share/apps/xpertmud/btech-maps");

  maps[current->getId()] = current;

  weaponStats = new WeaponStats();

  // first try this way
  secondaryParserSince = 0;
  primary = hudParser;
  secondary = NULL;

  QTimer *t =new QTimer(this);
  connect(t, SIGNAL(timeout()),
	  this, SLOT(slotTimer()));

  /* is prime and > 250 (preventing harmonics with Scripting timer) */
  t->start(257);
  
}

BattleCore::~BattleCore() {
  delete hudParser;
  hudParser=NULL;
//  delete textParser;
  textParser=NULL;
  current=NULL;
  for (mapsT::iterator it=maps.begin();
       it!=maps.end();
       ++it) {
    delete it->second;
    it->second=NULL;
  }

}

void BattleCore::updateMechsLOS(const map<QString, MechInfo>& currentMechs) {
  // Copy Stuff action thingy
  std::vector<MechInfo> updateMe;
  std::map<QString, MechInfo>::const_iterator new_it=currentMechs.begin();
  mechIteratorT old_it=getMechBegin();
  while (new_it != currentMechs.end() &&
	 old_it != getMechEnd()) {
    //cout << "new: " << new_it->first << "; old: " << old_it->first << endl;
    if(new_it->first == old_it->first) { 
      updateMe.push_back(old_it->second);
      updateMe.back().update(new_it->second);
      ++new_it;
      ++old_it;
    } else if(new_it->first < old_it->first) {
      updateMe.push_back(new_it->second);
      ++new_it;
    } else {
      updateMe.push_back(old_it->second);
      updateMe.back().setInLOS(false);
      ++old_it;
    }
  }
  while(new_it != currentMechs.end()) {
    setMechInfo(new_it->second);
    ++new_it;
  }
  while(old_it != getMechEnd()) {
    updateMe.push_back(old_it->second);
    updateMe.back().setInLOS(false);
    ++old_it;
  }
  for(std::vector<MechInfo>::iterator it = updateMe.begin();
      it != updateMe.end();++it) {
    setMechInfo(*it);
  }
  // Is this needed, or does a flush
  // at hundinfo gs suffice ?
  flushDisplay();
}

void BattleCore::setMechInfo(const MechInfo& mechInfo) {
  MechInfo oldMechInfo = getMechInfo(mechInfo.getId());
  mechInfos[mechInfo.getId().upper()] = mechInfo;

  if(mechInfo.getId() == getOwnId()) {
    if(mechInfo.getPos().getZ() != oldMechInfo.getPos().getZ())
      switch(mechInfo.getType()) {
      case 'N':
      case 'Y':
      case 'U':
      case 'V':
      case 'F':
      case 'A':
      case 'D':
	emit repaintCliffings();
	break;
      default:
	if ((mechInfo.hasStatus() && mechInfo.getStatus().contains("J"))){
	  emit repaintCliffings();
	}
      }

    if (oldMechInfo.hasStatus() && oldMechInfo.getStatus().contains("J") &&
	!mechInfo.getStatus().contains("J")) {
      // we finished the jump
      emit repaintCliffings();
    }
    if(mechInfo.hasType() != oldMechInfo.hasType() ||
       mechInfo.getType() != oldMechInfo.getType())
      emit repaintCliffings();
    if(mechInfo.hasHeatDissipation() && 
       (!oldMechInfo.hasHeatDissipation() || (oldMechInfo.getHeatDissipation() != mechInfo.getHeatDissipation())))
      emit heatDissipationChanged(mechInfo.getHeatDissipation());
    if(mechInfo.hasHeat() && (!oldMechInfo.hasHeat() || (oldMechInfo.getHeat() != mechInfo.getHeat())))
      emit heatChanged(mechInfo.getHeat());
    if(mechInfo.hasSpeed() && (!oldMechInfo.hasSpeed() || (oldMechInfo.getSpeed() != mechInfo.getSpeed())))
      emit speedChanged(mechInfo.getSpeed());
    if(mechInfo.hasDesiredSpeed() && 
       (!oldMechInfo.hasDesiredSpeed() || (oldMechInfo.getDesiredSpeed() != mechInfo.getDesiredSpeed())))
      emit desiredSpeedChanged(mechInfo.getDesiredSpeed());
    if(mechInfo.hasRunningSpeed() && 
       mechInfo.hasWalkingSpeed() &&
       mechInfo.hasBackingSpeed() && 
       (
	!oldMechInfo.hasRunningSpeed() ||
	!oldMechInfo.hasWalkingSpeed() ||
	!oldMechInfo.hasBackingSpeed() ||
	(oldMechInfo.getRunningSpeed() != mechInfo.getRunningSpeed()) ||
	(oldMechInfo.getWalkingSpeed() != mechInfo.getWalkingSpeed()) ||
	(oldMechInfo.getBackingSpeed() != mechInfo.getBackingSpeed())
	)
       )
      emit maxSpeedChanged(mechInfo.getRunningSpeed(),
			   mechInfo.getWalkingSpeed(),
			   mechInfo.getBackingSpeed());
  }
  /* Temporarily disabled 
     if ((!oldMechInfo.hasPos() || (oldMechInfo.getPos() != mechInfo.getPos())) ||
     (!oldMechInfo.hasHeading() || (oldMechInfo.getHeading() != mechInfo.getHeading()))) {
  */
  // TODO: enough checks?
  // TODO: if own Position changes, always update ALL other contacts?
  emit queueMechInfoChange(oldMechInfo, mechInfo);
  // }
}

void BattleCore::slotTimer() {
  primary->flush();
  /* Never flush secondary if the primary parser is active!!!!!!!
     Why not?
     The secondary parser doesn't do anything besides
     activating, which means settings some commands and
     Tacheight. If you don't set it's intervals (which you
     can't do, because the bindings automagically set them
     on the primary parser), it will not do anything after
     being initialized, so why not flush the secondary
     parser???
     Oh, I see the problem, there was a problem with
     the python script that won't give all the input
     to the text parser (it did give the input only in
     battlemode, which is sensible, but not until
     the parser is initialized).
  */
  if(secondary != NULL) {
    secondary->flush();
    if(secondary->isActive() && !primary->isActive()) {
      ++secondaryParserSince;
      if(secondaryParserSince > 15) {
	//	cout << "Setting Secondary as Primary!" << endl;
	emit log("Deactivating primary (hudinfo) parser. Activating secondary (text) parser.");
	primary = secondary;
	secondary = NULL;
      }
    }
  }
}

void  BattleCore::slotSetTacticalInterval(int time) {
  primary->setTacticalInterval(time);
}
void  BattleCore::slotSetStatusInterval(int time) {
  primary->setStatusInterval(time);
}
void  BattleCore::slotSetContactsInterval(int time) {
  primary->setContactsInterval(time);
}
void  BattleCore::slotSetWeaponsInterval(int time) {
  primary->setWeaponsInterval(time);
}

void BattleCore::slotParseHUD(const QString& toParse) {
  hudParser->parse(toParse);
}

void BattleCore::slotParseText(const QString& toParse) {
//  if (textParser && secondary) textParser->parse(toParse);
}

void BattleCore::startSecondary()  { /*if (textParser) secondary = textParser; */}

BattleCore * BattleCore::instance=NULL;

BattleCore * BattleCore::getInstance() {
  if (instance==NULL) {
    instance=new BattleCore();
  }
  ++(instance->refcount);
  return instance;
}

void BattleCore::returnInstance() {
  assert(instance != NULL);
  assert(instance->refcount > 0);
  --(instance->refcount);
  if (instance->refcount==0) {
    delete instance;
    instance=NULL;
  }
}

