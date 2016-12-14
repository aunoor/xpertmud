// -*- c++ -*-
#ifndef HUD_PARSER_H
#define HUD_PARSER_H

#include "BattleCore.h"
#include <qmap.h>

#define HUDINFO_VERSION "0.6"


class HUDParser: public BattleParser {
public:
  HUDParser(BattleCore * core):
    BattleParser(core),serverVersionRequested(0),
    gsTime(0), gsCount(0), sgiCount(0),gsProtect(false),
    tacTime(0), tacCount(0), tacProtect(false),
    conTime(0), conCount(0), conProtect(false),
    hasWeaponList(false),
    maxWeaponId(-1)
  {
    active = false;
  }

  void parse(const QString &);
  void flush();

  void setTacticalInterval(int time) { tacTime=time; tacCount=4; tacProtect=false; } // 0==disable
  void setStatusInterval(int time) { gsTime=time; gsCount=1; sgiCount=0; gsProtect=false; }
  void setContactsInterval(int time) { conTime=time; conCount=2; conProtect=false; }
  void setWeaponsInterval(int time) { weTime=time; weCount=3; weProtect=false; }
protected:
  void parseGeneralStatus(const QString &);
  void parseStaticGeneralInfo(const QString &);
  void parseContacts(const QString &);
  void parseTacticalStart(const QString &);
  void parseTacticalLine(const QString &);
  void parseWeaponListLine(const QString &);
  void parseWeaponStatusLine(const QString &);
  void parseOriginalArmorStatus(const QString &);
  void parseArmorStatus(const QString &);
protected:
  QString serverVersion;
  // How many flush's ago was the request?
  int serverVersionRequested;
  QString sessionKey;

  // The current hudinfo t region
  HEXPos tacLeftUpper;
  HEXPos tacRightLower;

  typedef map<QString, MechInfo> mechsT;
  mechsT currentMechs;

  QMap<QString, int> workOriginalArmor;
  QMap<QString, int> workCurrentArmor;


  int gsTime;
  int gsCount;
  int sgiCount;
  bool gsProtect;

  int tacTime;
  int tacCount;
  bool tacProtect;

  int conTime;
  int conCount;
  bool conProtect;

  int weTime;
  int weCount;
  bool weProtect;

  bool hasWeaponList;

  // context sensitive parsing information
  int maxWeaponId;
};


#endif
