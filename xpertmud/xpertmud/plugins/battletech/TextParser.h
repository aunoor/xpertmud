// -*- c++ -*-
#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include <qregexp.h>
#include <qstring.h>
#include "BattleCore.h"

class TextParser: public BattleParser {
  static const int LAG_PROTECT_TIME;
public:
  TextParser(BattleCore * core);

  void parse(const QString& text);
  void flush();

  void setTacticalInterval(int time) { tacTime=time; tacCount=1;} // 0==disable
  void setStatusInterval(int time) { gsTime=time; gsCount=1; }
  void setContactsInterval(int time) { conTime=time; conCount=1; }
  void setWeaponsInterval(int time) { weTime=time; weCount=1; }

protected:
  void parseTactical(const QString& text);
  void parseContacts(const QString& text);

  // status, temp, findcenter, speed, heading...
  void parseStatus(const QString& text);

  void lagProtect(int& count, int& time, bool& protect,
		  const QString& description);
  bool stepTimer(int& count, int time, bool& protect);

private:
  int gsTime;
  int gsCount;
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

  int initializeRequested;
  bool initialized;

  bool doTactical;
  bool doContacts;
  bool doStatus;

  bool inContacts;
  QRegExp contBegin;
  QRegExp contLine;
  QRegExp contEnd;
  typedef map<QString, MechInfo> mechsT;
  mechsT currentMechs;

  bool inTactical;
  int tactCurrentX;
  int tactCurrentY;
  QRegExp tactNumbers;
  QRegExp tactStart;
  QRegExp tactFirstNumber;
  QRegExp tactFirstEmpty;
  QRegExp tactEndLine;
  QRegExp tactUpperHex;
  QRegExp tactLowerHex;
  
  bool inWeapons;
  QRegExp statXYZExcessHeat;
  QRegExp statHeatSt;
  QRegExp statBeforeHeatSt;
  QRegExp statSpeedHeatDissipation;
  QRegExp statSpeedCurrent;
  QRegExp statHeadingSt;
  QRegExp statBeforeHeadingSt;
  QRegExp statHeatDissipation;
  QRegExp statBeforeHeatDissipation;
  QRegExp statSpeedInfo;
  QRegExp statBeforeSpeedInfo;
  QRegExp statDesSpeedInfo;
  QRegExp statMechInfo;
  QRegExp statBeforeName;
  QRegExp statBeforeId;
  QRegExp statFindcenter;
  QRegExp statHeat;
  QRegExp statSpeed;
  QRegExp statHeading;
  QRegExp statWeaponStart;
  QRegExp statWeaponLine;
  QRegExp statWeaponEnd;
  QRegExp weapStart;
  QRegExp weapEntry;
  QRegExp weapName;
  QRegExp weapBeforeNr;
  QRegExp weapBeforeLocation;
  QRegExp weapLocation;
  QRegExp weapBeforeStatus;
  QRegExp weapStatus;
  QRegExp weapAmmoBeforeName;
  QRegExp weapAmmoName;
  QRegExp weapAmmoBeforeStatus;
  QRegExp weapAmmoStatus;
  QRegExp weapAmmoBeforeAmount;
  QRegExp weapAmmoAmount;
  QRegExp wsLine;
  QRegExp wsName;
  QRegExp wsSpace;
  QRegExp wsNumber;

  QRegExp initExp;

  QRegExp spaces;

  int maxWeaponId;
};

#endif
