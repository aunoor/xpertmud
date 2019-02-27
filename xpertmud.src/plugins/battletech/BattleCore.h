// -*- c++ -*-
#ifndef BATTLECORE_H
#define BATTLECORE_H

#include "Positions.h"
#include "MechInfo.h"
#include "WeaponInfo.h"

#include <algorithm>
using std::max;
using std::min;

#include <map>
using std::map;

#include <QString>

// TODO: remove if un-inline done
#include <QTextStream>

#include <QDateTime>
#include <QFile>
#include <QVector>

#include <kurl.h>

#ifndef ABSTRACT
#  define ABSTRACT 0
#endif


class MapTile {
public:
  // TODO: using an flag for UNKNOWN_HEIGHT is quite inefficient.
  // Better set height to -1 (or (int)'?') for that

  static const char UNKNOWN_HEIGHT; // = 1;

  enum ETile {
    HEAVY_WOODS = 34, // ", evil qt windows bug
   ROAD = '#',
   ROUGH = '%',
   FIRE = '&',
   SNOW = '+',
   ICE = '-',
   PLAINS = '.',
   BRIDGE = '/',
   BROKEN_BRIDGE = '$',
   SMOKE = ':',
   SMOKE2 = ';',
   WALL = '=',
//   PAVEMENT = '=',
   UNKNOWN = '?',
   BUILDING = '@',
   MOUNTAIN = '^',
   LIGHT_WOODS = '`',
   WATER = '~',
   DESERT = '}'
  };

  enum EKind {
     UNKNOWN_KIND,
     ROAD_KIND,
     BUILDING_KIND,
     WATER_KIND,
     WOODS_KIND,
     PLAIN_KIND,
     MOUNTAIN_KIND
  };

  MapTile():
    type(UNKNOWN), height(0), flags(UNKNOWN_HEIGHT),surroundings(0) {}

  MapTile(char t, char height):
    type(t), height(height), flags(0),surroundings(0) {
    if (type == '\'')
      type= '`';
  }
  
  MapTile(QTextStream & ts);

  bool hasHeight() const { return !(flags & UNKNOWN_HEIGHT); }
  int getRealHeight() const { if(type==(char)WATER)return -1*height; else if(type==(char)ICE)return 0; else return height; }
  int getHeight() const { return height; }
  void setHeight(int h) {
    if (h >= 0 && h <= 9) {
      height=h;
      flags &= ~UNKNOWN_HEIGHT;
    } else { 
      height=0;
      flags |= UNKNOWN_HEIGHT;
    }
  }

  bool hasType() const { return (int)type != UNKNOWN; }
  char getType() const { return type; }
  void setType(char t) { 
    type=t;
    if (type == '\'')
      type= '`';
    // TODO: add checks and set to UNKNOWN..
  } 

  EKind getKind() const {
     switch (type) {
	case ROAD:
	case BRIDGE:
	case BROKEN_BRIDGE:
 		return ROAD_KIND;
	case BUILDING:
	case WALL:
		return BUILDING_KIND;
	case WATER:
	case ICE:
		return WATER_KIND;
	case LIGHT_WOODS:
	case HEAVY_WOODS:
		return WOODS_KIND;
	case PLAINS:
	case DESERT:
	case SNOW:
		return PLAIN_KIND;
	case ROUGH:
	case MOUNTAIN:
		return MOUNTAIN_KIND;
	default:
		return UNKNOWN_KIND;
     }
   }

  bool operator==(const MapTile& tile) {
    return (type == tile.type) &&
      (height == tile.height) &&
      (flags == tile.flags) &&
      hasType() && hasHeight();
  }
  bool operator!=(const MapTile& tile) {
    return (type != tile.type) ||
      (height != tile.height) ||
      (flags != tile.flags);
  }

  void setSurroundings(unsigned int s) {
    surroundings=s|0x3F;
  }
  unsigned int getSurroundings() const {
    return surroundings;
  }
  void setSameFlagN() {
    surroundings |= 0x01;
  }
  void setSameFlagNE() {
    surroundings |= 0x02;
  }
  void setSameFlagSE() {
    surroundings |= 0x04;
  }
  void setSameFlagS() {
    surroundings |= 0x08;
  }
  void setSameFlagSW() {
    surroundings |= 0x10;
  }
  void setSameFlagNW() {
    surroundings |= 0x20;
  }
 
  void resetSameFlagN() {
    surroundings &= ~0x01;
  }
  void resetSameFlagNE() {
    surroundings &= ~0x02;
  }
  void resetSameFlagSE() {
    surroundings &= ~0x04;
  }
  void resetSameFlagS() {
    surroundings &= ~0x08;
  }
  void resetSameFlagSW() {
    surroundings &= ~0x10;
  }
  void resetSameFlagNW() {
    surroundings &= ~0x20;
  }

private:
  char type;
  char height;
  unsigned char flags;
  // Bitmap of which surrounding Tiles are of the same type
  unsigned char surroundings;
};

QTextStream & operator<<(QTextStream & ts, const MapTile t);
QTextStream & operator>>(QTextStream & ts, MapTile & t);

//////////////////////////////////////////////////////////////////////

class BattleCore;

class BattleParser {
public:
  BattleParser(BattleCore * core):core(core) {}
  virtual ~BattleParser() {};
  virtual void parse(const QString& text) = ABSTRACT;

  bool isActive() { return active; }
  
  // send out pending commands
  virtual void flush() = ABSTRACT;
  
  // 0==disable
  virtual void setTacticalInterval(int time) = ABSTRACT;
  virtual void setStatusInterval(int time) = ABSTRACT;
  virtual void setContactsInterval(int time) = ABSTRACT;
  virtual void setWeaponsInterval(int time) = ABSTRACT;

protected:
  BattleCore * core;
  bool active;
};

//////////////////////////////////////////////////////////////////////

class BattleMap {
  typedef QVector< QVector<MapTile> > mapT;
public:
  BattleMap(const QString& id, unsigned int width=0, unsigned int height=0);
  BattleMap(const QString& id, QTextStream& text);
  void resize(unsigned int width, unsigned int height);
  void save(QTextStream& text);
  void load(QTextStream& text);
  void clear() {
  	width=0;
  	height=0;
  	myMap.clear();
  }
  // returns true if really changed
  bool setMapTile(const HEXPos & pos, const MapTile& mapTile);

  MapTile getMapTile(const HEXPos & pos) const {
    if(pos.getY()<0 || pos.getY() >= (int)myMap.size())
      return MapTile();
    if(pos.getX()<0 || pos.getX() >= (int)myMap[pos.getY()].size())
      return MapTile();
    return myMap[pos.getY()][pos.getX()];
  }

  const QString& getId() const { return id; }
  unsigned int getWidth() const { return width; }
  unsigned int getHeight() const { return height; }
  
private:
  QString id;
  mapT myMap;
  unsigned int width;
  unsigned int height;
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class TextParser;
class HUDParser;

class BattleCore: public QObject {
  Q_OBJECT
  typedef map<QString, BattleMap *> mapsT;

  // mustn't go into map, because it's just temporary
  // and will not be saved anywhere
  typedef map<QString, MechInfo> mechsT;

  typedef QVector<WeaponInfo> weaponsT;
public:
  typedef mechsT::const_iterator mechIteratorT;

  // MAP
  void addMap(BattleMap* map) {
    if(maps.find(map->getId()) != maps.end() && maps[map->getId()] != map)
      delete maps[map->getId()];
    maps[map->getId()] = map;
  }
  void setCurrentMap(const QString& id) {
    if(maps.find(id) != maps.end()) {
      current = maps[id];
    } else {
      addMap(new BattleMap(id));
    }
  }
  void loadMap(const KURL& url) {
    if(!url.isLocalFile())
      return;
    QFile f(url.directory()+'/'+url.fileName());
    if(!f.open(QIODevice::ReadOnly))
      return;
    {
      unsigned int oldW=current->getWidth();
      unsigned int oldH=current->getHeight();
      QTextStream fs(&f);
      setCurrentMap(url.fileName());
      current->load(fs);
      emit queueMapChange(HEXPos(0, 0), HEXPos(max(oldW,current->getWidth())-1,
					       max(oldH,current->getHeight())-1));
      emit flushDisplayChange();
    }
    f.close();
  }
  void clearMap() {
	unsigned int oldW=current->getWidth();
  	unsigned int oldH=current->getHeight();
  	current->clear();
  	emit queueMapChange(HEXPos(0, 0), HEXPos(oldW,oldH));
  	emit flushDisplayChange();
  	                                                                       
  }
  void saveMap(const KURL& url) {
    if(!url.isLocalFile())
      return;
    QFile f(url.directory()+'/'+url.fileName());
    if(!f.open(QIODevice::WriteOnly))
      return;
    {
      QTextStream fs(&f);
      current->save(fs);
    }
    f.close();
  }

  void setMapTile(const HEXPos & pos, const MapTile& mapTile, bool doEmit=true) {
    if (current->setMapTile(pos, mapTile) && doEmit)
      emit queueMapChange(pos, pos);
  }

  void flushDisplay() {
    emit flushDisplayChange();
  }


  MapTile getMapTile(const HEXPos & pos) const {
    return current->getMapTile(pos);
  }

  unsigned int getMapWidth() const { return current->getWidth(); }
  unsigned int getMapHeight() const { return current->getHeight(); }


  /// MECH-INFO vom Parser
  void setMechInfo(const MechInfo& mechInfo);

  // Mech info vom Script!
  void setMechInfo(const QString &) {
    // TODO: compare ages,
    // dont overwrite newer info
    // Try to keep allready known info
    // etc...
    // We'll add this to the MechInfo::update() method
  }
  
  void deleteMechInfo(const QString &id) {
    mechsT::iterator mi = mechInfos.find(id.toUpper());
    MechInfo oldMechInfo;
    if(mi != mechInfos.end()) {
      oldMechInfo=mi->second;
      mechInfos.erase(mi);
      emit queueMechInfoChange(oldMechInfo, MechInfo());
    }
  }

  void flushMechInfo() {
    emit flushDisplayChange();
  }

  void updateMechsLOS(const map<QString, MechInfo>& toUpdate);

  MechInfo getMechInfo(const QString& id) const {
    mechsT::const_iterator mi = mechInfos.find(id.toUpper());
    if(mi != mechInfos.end())
      return mi->second;
    return MechInfo();
  }
  void setVisibleOverride(const QString & id, bool override, bool vis) {
    mechsT::iterator mi = mechInfos.find(id.toUpper());
      if(mi != mechInfos.end())
        mi->second.setVisibleOverride(override, vis);
  }
  void setOwnId(const QString& id) {
    ownId = id;
  }
  const QString & getOwnId() const {
    return ownId;
  }
  mechIteratorT getMechBegin() const {
    return mechInfos.begin();
  }
  mechIteratorT getMechEnd() const {
    return mechInfos.end();
  }

  // weapon stuff
  int getNrWeapons() const { return weaponInfos.size(); }
  void setNrWeapons(int nr) { 
    weaponInfos.resize(nr); 
    emit nrWeaponsChange(nr);
  }
  WeaponInfo getWeaponInfo(int id) { 
    if((int)weaponInfos.size() <= id)
      weaponInfos.resize(id+1);
    return weaponInfos[id]; 
  }
  void setWeaponInfo(int id, const WeaponInfo& wi) {
    if((int)weaponInfos.size() <= id)
      weaponInfos.resize(id+1);
    weaponInfos[id] = wi;
    emit weaponChange(id);
  }
  void addWeaponStat(WeaponStat& ws) {
    weaponStats->addStat(ws);
  }
  WeaponStat getWeaponStat(int id) { return weaponStats->getStat(id); }
  WeaponStat getWeaponStat(const QString& name) { return weaponStats->getStat(name); }

  static BattleCore * getInstance();
  static void returnInstance();

protected:
  BattleCore();
  ~BattleCore();

signals:

  // Request an Update of all widgets that display speed
  void maxSpeedChanged(double maxRunningSpeed,
		       double maxWalkingSpeed, 
		       double maxBackingspeed);

  // Request an Update of all widgets that display speed
  void speedChanged(double speed);
  
  void desiredSpeedChanged(double desiredSpeed);

  // Request an Update of all widgets that display heat
  void heatChanged(int heat);

  // Request an Update of all widgets that require the number of heatsinks
  void heatDissipationChanged(int heat);

  // Request to send commands to the mud. should be only connected once...
  void send(const QString &);

  // Request a repaint of all cliffs
  void repaintCliffings();

  // Request to all mapViews to update the given area
  //void mapChange(const HEXPos & from, const HEXPos & to);

  // Request to all mapViews and contact widgets to schedule
  // the change at next Update
  void queueMechInfoChange(const MechInfo& oldInfo, const MechInfo& mechInfo);

  // Request to all mapViews to apply all scheduled changes;
  void flushDisplayChange();

  // Request to MapViews to update given HEX
  //  void mapChanged(const HEXPos & pos);

  
  void queueMapChange(const HEXPos & fromPos, const HEXPos & toPos);

  void weaponChange(int id);
  void nrWeaponsChange(int nr);

  // Send data to log..
  void log(const QString &);

public:

protected slots:
  void slotTimer();

public slots:
  void slotSend(const QString & cmd) { emit send(cmd); }
  void slotLog(const QString & line) { emit log(line); }
  void slotParse(const QString & parse) { 
    primary->parse(parse);
  }
  
  void slotSetTacticalInterval(int time); // 0==disable
  void slotSetStatusInterval(int time);
  void slotSetContactsInterval(int time);
  void slotSetWeaponsInterval(int time);

  void slotParseHUD(const QString& toParse);
  void slotParseText(const QString& toParse);

  void startSecondary();

private: // data
  mapsT maps;
  mechsT mechInfos;

  weaponsT weaponInfos;
  WeaponStats *weaponStats;

  unsigned int refcount; 

  BattleParser * primary; // parsing and command sending
  BattleParser * secondary; // for parsing only

  // to access special singular abilities
  HUDParser *hudParser; 
  TextParser *textParser;
  int secondaryParserSince;

  BattleMap *current;

  static BattleCore * instance;
  QString ownId;
};

#endif
